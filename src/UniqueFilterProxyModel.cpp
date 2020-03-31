#include "UniqueFilterProxyModel.hpp"

#include <QDebug>

namespace sempr { namespace gui {

UniqueFilterProxyModel::UniqueFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

void UniqueFilterProxyModel::buildMap()
{
    beginResetModel();

    valueToIndex_.clear();
//    qDebug() << "clear map";
    if (sourceModel())
    {
        int rowCount = sourceModel()->rowCount();
//        qDebug() << "iterate source model " << sourceModel() << " with " << rowCount << " rows";
        for (int r = 0; r < rowCount; r++)
        {
            auto index = sourceModel()->index(r, 0);
            QVariant val = index.data();
            if (valueToIndex_.find(val) == valueToIndex_.end())
            {
                valueToIndex_[val] = index;
//                qDebug() << "added " << val << " to map";
            }
        }
    }

    endResetModel();
    invalidate();
}


bool UniqueFilterProxyModel::filterAcceptsRow(
        int sourceRow,
        const QModelIndex& sourceParent) const
{
    QPersistentModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QString value = sourceModel()->data(index).toString();

    // find the value in the map
    auto it = valueToIndex_.find(value);

    if (it != valueToIndex_.end())
    {
        // same index as the first found with the value -> all good.
        if (*it == index) return true;
    }
    else
    {
        // seems to be a normal case, that the filterAcceptsRow is called
        // even befor it iterated over a valid source model...
        // just reject everything, it is queried for again later on.
        //qDebug() << "wtf? value " << value << " not found in map!";
        // something went wrong, the map wasn't constructed correctly...
        //throw std::exception();
    }

    return false;
}



void UniqueFilterProxyModel::setSourceModel(QAbstractItemModel* source)
{
    if (sourceModel())
    {
        disconnect(sourceModel(), &QAbstractItemModel::layoutChanged,
                   this, &UniqueFilterProxyModel::buildMap);
        disconnect(sourceModel(), &QAbstractItemModel::modelReset,
                   this, &UniqueFilterProxyModel::buildMap);
        disconnect(sourceModel(), &QAbstractItemModel::dataChanged,
                   this, &UniqueFilterProxyModel::buildMap);
    }

    QSortFilterProxyModel::setSourceModel(source);

    connect(source, &QAbstractItemModel::layoutChanged,
            this, &UniqueFilterProxyModel::buildMap);
    connect(source, &QAbstractItemModel::modelReset,
            this, &UniqueFilterProxyModel::buildMap);
    connect(source, &QAbstractItemModel::dataChanged,
            this, &UniqueFilterProxyModel::buildMap);

    buildMap();
}

}}
