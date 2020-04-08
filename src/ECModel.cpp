#include "ECModel.hpp"
#include "CustomDataRoles.hpp"

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
        [this](ECData entry, AbstractInterface::Notification n)
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
                    std::cout << "Dummy update callback add " << entry.componentId << std::endl;
                    this->emit gotEntryAdd(entry);
                    break;
                case AbstractInterface::UPDATED:
                    std::cout << "Dummy update callback updated " << entry.componentId << std::endl;
                    this->emit gotEntryUpdate(entry);
                    break;
                case AbstractInterface::REMOVED:
                    std::cout << "Dummy update callback removed " << entry.componentId << std::endl;
                    this->emit gotEntryRemove(entry);
                    break;
            }
        }
    );
    // Initialize by retrieving all existing data
    auto entries = semprInterface_->listEntityComponentPairs();
    // Sort by entity id first.
    std::sort(entries.begin(), entries.end(),
            [](const ECData& left, const ECData& right)
            {
                return left.entityId < right.entityId;
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
    return findEntry(entry.entityId(), entry.componentId());
}

QModelIndex ECModel::findEntry(const std::string& entityId,
                               const std::string& componentId) const
{
    // find the row of the entity-group
    auto group = std::find_if(data_.begin(), data_.end(),
        [&entityId](const ModelEntryGroup& group)
        {
            return group.entityId_ == entityId;
        }
    );

    if (group != data_.end())
    {
        int entityRow = std::distance(data_.begin(), group);
        auto parent = this->index(entityRow, 0, QModelIndex());

        // find the entry in the group
        auto component = std::find_if(group->entries_.begin(), group->entries_.end(),
            [&componentId](const ModelEntry& other)
            {
                return componentId == other.componentId();
            }
        );
        if (component != group->entries_.end())
        {
            int componentRow = std::distance(group->entries_.begin(), component);
            auto index = parent.child(componentRow, 0);
            return index;
        }
    }

    return QModelIndex();
}


void ECModel::addModelEntry(const ECData& entry)
{
    // This can happen e.g. if data was added between setting the callback
    // for updates and the initialization of the model.
    auto index = this->findEntry(entry.entityId, entry.componentId);
    if (index.isValid()) return;

    // find the entity-group
    auto entity = std::find_if(data_.begin(), data_.end(),
        [&entry](const ModelEntryGroup& group) -> bool
        {
            return group.entityId_ == entry.entityId;
        }
    );

    if (entity != data_.end())
    {
        // the group exists, to lets add the entry
        int entityRow = std::distance(data_.begin(), entity);
        int componentRow = entity->entries_.size();

        // index of the parent where the row is inserted
        auto parent = this->index(entityRow, 0, QModelIndex());

        // and we are going to insert at the end of the component list
        this->beginInsertRows(parent, componentRow, componentRow);

        // now, insert:
        entity->entries_.push_back(ModelEntry(entry));

        // finish insertion
        this->endInsertRows();
    }
    else
    {
        // the group does not exist yet, so lets insert it.
        int entityRow = data_.size();

        // parent is the root item
        QModelIndex parent;

        this->beginInsertRows(parent, entityRow, entityRow);
        ModelEntryGroup group;
        group.entityId_ = entry.entityId;
        group.entries_.push_back(ModelEntry(entry));
        data_.push_back(group);
        this->endInsertRows();
    }
}


void ECModel::removeModelEntry(const ECData& entry)
{
    auto index = this->findEntry(entry.entityId, entry.componentId);
    if (index.isValid())
    {
        // signal removal of this entry
        this->beginRemoveRows(index.parent(), index.row(), index.row());
        // find the group
        auto group = data_.begin() + index.parent().row();
        // find the component
        auto component = group->entries_.begin() + index.row();
        // remove the entry
        group->entries_.erase(component);
        // signal end of removal
        this->endRemoveRows();

        // if this was the last entry, remove the entity-entry all together!
        // (at some points we make the assumption that there is always at least
        // one entry in every group!)
        if (group->entries_.empty())
        {
            this->beginRemoveRows(QModelIndex(), index.parent().row(), index.parent().row());
            data_.erase(group);
            this->endRemoveRows();
        }
    }
}

void ECModel::updateModelEntry(const ECData& entry)
{
    // find the entries index
    auto index = this->findEntry(entry.entityId, entry.componentId);

    // get the group
    auto group = data_.begin() + index.parent().row();
    // get the entry
    auto component = group->entries_.begin() + index.row();
    *component = ModelEntry(entry);

    // notify views
    this->dataChanged(index, index);
}


void ECModel::commit()
{
    for (auto& group : data_)
    {
        for (auto& entry : group.entries_)
        {
            if (entry.isModified())
            {
                updateComponent(entry);
            }
        }
    }
}

void ECModel::reset()
{
    for (auto& group : data_)
    {
        for (auto& entry : group.entries_)
        {
            if (entry.isModified())
            {
                entry.setJSON(entry.coreData_.componentJSON);
                auto index = findEntry(entry);
                emit dataChanged(index, index);
            }
        }
    }
}


void ECModel::addComponent(const ModelEntry& entry)
{
    ECData data;
    data.entityId = entry.entityId();
    data.componentId = entry.componentId();
    data.componentJSON = entry.json();

    semprInterface_->addEntityComponentPair(data);
}

void ECModel::removeComponent(const ModelEntry& entry)
{
    ECData data;
    data.entityId = entry.entityId();
    data.componentId = entry.componentId();

    semprInterface_->removeEntityComponentPair(data);
}

void ECModel::updateComponent(const ModelEntry& entry)
{
    ECData data;
    data.entityId = entry.entityId();
    data.componentId = entry.componentId();
    data.componentJSON = entry.json();

    semprInterface_->modifyEntityComponentPair(data);
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

    // most roles can only be served for ModelEntrys, not ModelEntryGroups.
    // -> parent must be valid.
    // Hence: First check if thet parent is not valid, and serve only the
    // Qt::DisplayRole.
    if (!parent.isValid())
    {
        if (role == Qt::DisplayRole)
        {
            // top level -> entity id
            return QString::fromStdString(data_[index.row()].entityId_);
        }
        // no other roles served for top level items.
        return QVariant();
    }

    // --- only ModelEntry below this line ---
    auto& entry = data_[parent.row()].entries_[index.row()];
    if (role == Qt::DisplayRole)
    {
        // "*" (if modified) + "Component_" + componentId.
        QString display("%1Component_%2");
        display = display.arg(entry.isModified() ? "*" : "")
                         .arg(QString::fromStdString(entry.componentId()));
        return display;
    }
    else if (role == Role::EntityIdRole)
    {
        return QString::fromStdString(entry.entityId());
    }
    else if (role == Role::ComponentIdRole)
    {
        return QString::fromStdString(entry.componentId());
    }
    else if (role == Role::ComponentJsonRole)
    {
        return QString::fromStdString(entry.json());
    }
    else if (role == Role::ComponentPtrRole)
    {
        return QVariant::fromValue(entry.component());
    }
    else if (role == Role::ComponentMutableRole)
    {
        return QVariant::fromValue(entry.isComponentMutable());
    }
    else if (role == Role::ModelEntryRole)
    {
        return QVariant::fromValue(entry);
    }
    else if (role == Qt::BackgroundRole)
    {
        // grey background if the component is not mutable
        if (!entry.isComponentMutable())
        {
            return QColor::fromRgb(147, 161, 161);
        }
        else if (entry.isModified())
        {
            return QColor::fromRgb(181, 137, 0);
        }
        else
        {
            return QColor::fromRgb(255, 255, 255);
        }
    }

    return QVariant();
}


bool ECModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid()) return false;
    if (!index.parent().isValid()) return false;

    auto& entry = data_[index.parent().row()].entries_[index.row()];
    if (role == Role::ComponentJsonRole && value.canConvert<QString>())
    {
        entry.setJSON(value.value<QString>().toStdString());
        emit dataChanged(index, index);
        return true;
    }
    else if (role == Role::ComponentPtrRole && value.canConvert<Component::Ptr>())
    {
        // TODO: Don't really *want* to change the pointer! Noone is supposed to
        // do that! Only *update* the one that is already there.
        // So, actually, this is only supposed to trigger a "hey I changed
        // something that this pointer points to".
        auto ptr = value.value<Component::Ptr>();
        if (ptr == entry.component_)
        {
            // only on pointer-equality, do something.
            entry.componentPtrChanged();
            emit dataChanged(index, index);
            return true;
        }
        return false;
    }

    return false;
}


QVariant ECModel::headerData(int, Qt::Orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        return "Id";
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
        // an internalId of zero indicates that this is a first-level entry,
        // i.e. is group, an entity.
        if (parent.internalId() == 0)
        {
            // --> number of components in the entity
            return data_[parent.row()].entries_.size();
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
    // the internalId stores the groupId of the ModelEntryGroup that the parent
    // points to. This allows us to find the correct parent of an index even
    // after the parent has been moved, as opposed to using simply the row of
    // the parent as the internalId.
    int groupId = data_[parent.row()].groupId_;

    if (parent.isValid())
        return this->createIndex(row, col, groupId);
    // if the parent is not valid, i.e. we want to create an index that points
    // at a ModelEntryGroup, we just set the internalId to zero.
    return this->createIndex(row, col, quintptr(0));
}

QModelIndex ECModel::parent(const QModelIndex& index) const
{
    // if the internalId is 0 this index point at a first-level entry, and thus
    // the parent is the root entry -> invalid QModelIndex.
    if (index.internalId() == 0)
        return QModelIndex();

    // else, we need to find the row in data_ where the ModelEntryGroup with the
    // groupId as specified in the internalId is located.
    auto it = std::find_if(data_.begin(), data_.end(),
        [&index](const ModelEntryGroup& group)
        {
            return group.groupId_ == index.internalId();
        }
    );

    if (it == data_.end())
    {
        // this should not happen. The Index we wanted to get the parent for
        // is not in the datastructure. :(
        throw std::exception();
    }

    int row = std::distance(data_.begin(), it);
    return createIndex(row, 0, quintptr(0));
}

}}
