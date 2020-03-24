#include "ECModel.hpp"

#include <iostream>

namespace sempr { namespace gui {


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
