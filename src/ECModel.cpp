#include "ECModel.hpp"

#include <QColor>
#include <thread>
#include <iostream>

namespace sempr { namespace gui {

ECModel::ECModel(AbstractInterface::Ptr interface)
    : semprInterface_(interface)
{
    connect(this, &ECModel::gotEntryAdd,
            this, &ECModel::addModelEntry);
    connect(this, &ECModel::gotEntryUpdate,
            this, &ECModel::updateModelEntry);
    connect(this, &ECModel::gotEntryRemove,
            this, &ECModel::removeModelEntry);

    // TODO: Register callback for updates
    semprInterface_->setUpdateCallback(
        [this](ModelEntry entry, AbstractInterface::Notification n)
        {

            // DONT call addModelEntry etc from this callback, as it
            // will call it from the wrong thread. Might not sound like
            // a big deal, as it does not directly do any UI stuff,
            // BUT:
            // beginInsertRows(...) etc. emit signals that need to be processed
            // by the views *BEFORE* data is changed! I'm not entirely sure if
            // I understood the docs correctly, but to be sure: Lets emit
            // a signal instead (emitting signals is thread safe), and connect
            // it to the addModelEntry-slot (etc). As the ECModel lives in the
            // gui thread its events are handled there, and everything should
            // be fine.
            //

            switch (n) {
                case AbstractInterface::ADDED:
                    std::cout << "Dummy update callback add " << entry.componentId_ << std::endl;
                    this->emit gotEntryAdd(entry);
                    break;
                case AbstractInterface::UPDATED:
                    this->emit gotEntryUpdate(entry);
                    break;
                case AbstractInterface::REMOVED:
                    this->emit gotEntryRemove(entry);
                    break;
            }
        }
    );
    // Initialize by retrieving all existing data
    auto entries = semprInterface_->listEntityComponentPairs();
    // Sort by entity id first.
    std::sort(entries.begin(), entries.end(),
            [](const ModelEntry& left, const ModelEntry& right)
            {
                return left.entityId_ < right.entityId_;
            }
    );
    // add them one by one, groups them by entityId.
    for (auto& e : entries)
    {
        addModelEntry(e);
    }
}

ECModel::~ECModel()
{
    semprInterface_->clearUpdateCallback();
    std::cout << "~ECModel" << std::endl;
}


QModelIndex ECModel::findEntry(const ModelEntry& entry) const
{
    // find the row of the entity-group
    auto group = std::find_if(data_.begin(), data_.end(),
        [&entry](const ModelEntryGroup& group)
        {
            return group[0].entityId_ == entry.entityId_;
        }
    );

    if (group != data_.end())
    {
        int entityRow = std::distance(data_.begin(), group);
        auto parent = this->index(entityRow, 0, QModelIndex());

        // find the entry in the group
        auto component = std::find_if(group->begin(), group->end(),
            [&entry](const ModelEntry& other)
            {
                return entry.componentId_ == other.componentId_;
            }
        );
        if (component != group->end())
        {
            int componentRow = std::distance(group->begin(), component);
            auto index = parent.child(componentRow, 0);
            return index;
        }
    }

    return QModelIndex();
}


void ECModel::addModelEntry(const ModelEntry& entry)
{
    // This can happen e.g. if data was added between setting the callback
    // for updates and the initialization of the model.
    auto index = this->findEntry(entry);
    if (index.isValid()) return;

    // find the entity-group
    auto entity = std::find_if(data_.begin(), data_.end(),
        [&entry](const ModelEntryGroup& group) -> bool
        {
            return group[0].entityId_ == entry.entityId_;
        }
    );

    if (entity != data_.end())
    {
        int entityRow = std::distance(data_.begin(), entity);
        int componentRow = entity->size();

        // index of the parent where the row is inserted
        auto parent = this->index(entityRow, 0, QModelIndex());

        // and we are going to insert at the end of the component list
        this->beginInsertRows(parent, componentRow, componentRow);

        // now, insert:
        entity->push_back(entry);

        // finish insertion
        this->endInsertRows();
    }
    else
    {
        // the entity does not exist yet, so lets insert it.
        int entityRow = data_.size();

        // parent is the root item
        QModelIndex parent;

        this->beginInsertRows(parent, entityRow, entityRow);
        ModelEntryGroup group;
        group.push_back(entry);
        data_.push_back(group);
        this->endInsertRows();
    }
}


void ECModel::removeModelEntry(const ModelEntry& entry)
{
    auto index = this->findEntry(entry);
    if (index.isValid())
    {
        // signal removal of this entry
        this->beginRemoveRows(index.parent(), index.row(), index.row());
        // find the group
        auto group = data_.begin() + index.parent().row();
        // find the component
        auto component = group->begin() + index.row();
        // remove the entry
        group->erase(component);
        // signal end of removal
        this->endRemoveRows();

        // if this was the last entry, remove the entity-entry all together!
        // (at some points we make the assumption that there is always at least
        // one entry in every group!)
        if (group->empty())
        {
            this->beginRemoveRows(QModelIndex(), index.parent().row(), index.parent().row());
            data_.erase(group);
            this->endRemoveRows();
        }
    }
}

void ECModel::updateModelEntry(const ModelEntry& entry)
{
    // find the entries index
    auto index = this->findEntry(entry);
    // get the group
    auto group = data_.begin() + index.parent().row();
    // get the entry
    auto component = group->begin() + index.row();
    *component = entry;

    // notify views
    this->dataChanged(index, index);
}

void ECModel::addComponent(const ModelEntry& entry)
{
    semprInterface_->addEntityComponentPair(entry);
}

void ECModel::removeComponent(const ModelEntry& entry)
{
    semprInterface_->removeEntityComponentPair(entry);
}

void ECModel::updateComponent(const ModelEntry& entry)
{
    removeComponent(entry);
    addComponent(entry);
}



Qt::ItemFlags ECModel::flags(const QModelIndex&) const
{
    // All items are enabled and selectable.
    // Question is: How do we deal with editable?
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ECModel::data(const QModelIndex& index, int role) const
{
    auto parent = index.parent();

    if (role == Qt::DisplayRole)
    {
        if (!parent.isValid())
        {
            // top level -> entity id
            return QString::fromStdString(data_[index.row()][0].entityId_);
        }
        else
        {
            auto& entry = data_[parent.row()][index.row()];
            return QString::fromStdString("Component_" + entry.componentId_); // TODO: Need to get something sensible to display.
        }
    }
    else if (role == Qt::UserRole)
    {
        if (parent.isValid())
        {
            auto& entry = data_[parent.row()][index.row()];
            return QVariant::fromValue(entry);
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (parent.isValid())
        {
            auto& entry = data_[parent.row()][index.row()];
            return (entry.mutable_ ? QVariant() : QColor::fromHsl(0, 0, 180));
        }
    }

    return QVariant();
}

QVariant ECModel::headerData(int, Qt::Orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        return "foo";
    }

    return QVariant();
}

int ECModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
    {
        // index invalid -> root item -> number of entities
        return data_.size();
    }
    else
    {
        // index is valid
        if (parent.internalId() == 0)
        {
            // and internalId indicates that it is a first level entry
            // --> number of components in the entity
            return data_[parent.row()].size();
        }
        else
        {
            // a different internalId indicates that the index refers to a
            // component, and thus there are no rows beneath it
            return 0;
        }
    }
}

int ECModel::columnCount(const QModelIndex&) const
{
    return 1;
}


QModelIndex ECModel::index(int row, int col, const QModelIndex& parent) const
{
    // the internal id stores the row of the parent (+1) so that we can easily
    // find the parent of this index. The internal id is set to 0 if there is
    // no parent.

    if (parent.isValid())
        return this->createIndex(row, col, parent.row()+1);
    return this->createIndex(row, col, quintptr(0));
}

QModelIndex ECModel::parent(const QModelIndex& index) const
{
    if (index.internalId() == 0)
        return QModelIndex();
    return createIndex(index.internalId()-1, 0, quintptr(0));
}

}}
