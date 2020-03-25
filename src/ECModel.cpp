#include "ECModel.hpp"

#include <thread>
#include <iostream>

namespace sempr { namespace gui {

ECModel::ECModel(AbstractInterface::Ptr interface)
    : semprInterface_(interface)
{
    connect(this, &ECModel::gotEntryAdd,
            this, &ECModel::addModelEntry);

    // TODO: Register callback for updates
    semprInterface_->setUpdateCallback(
        [this](ModelEntry entry, AbstractInterface::Notification n)
        {
            std::cout << "Dummy update callback" << std::endl;

            // TODO:
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
            std::cout << "callback called in thread: " << std::this_thread::get_id() << std::endl;

            if (n == AbstractInterface::ADDED)
                this->emit gotEntryAdd(entry);
        }
    );
    // TODO: Initialize by retrieving all existing data
}


void ECModel::addModelEntry(const ModelEntry& entry)
{
    std::cout << "addModelEntry called in thread: " << std::this_thread::get_id() << std::endl;

    // find the entity
    auto entity = std::find_if(data_.begin(), data_.end(),
        [&entry](const ModelEntryGroup& group) -> bool
        {
            return group[0].entityId_ == entry.entityId_;
        }
    );

    if (entity != data_.end())
    {
        int entityRow = std::distance(data_.begin(), entity);
        // TODO: Check if component already present? Ah, don't care...
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
    // TODO
}
void ECModel::updateModelEntry(const ModelEntry& entry)
{
    // TODO
}

void ECModel::addComponent(const ModelEntry& entry)
{
    // TODO
}

void ECModel::removeComponent(const ModelEntry& entry)
{
    // TODO
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
            //auto& entry = data_[parent.row()][index.row()];
            return "Component"; // TODO: Need to get something sensible to display.
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
