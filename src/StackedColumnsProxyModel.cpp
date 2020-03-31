#include "StackedColumnsProxyModel.hpp"

#include <QDebug>

namespace sempr { namespace gui {

StackedColumnsProxyModel::StackedColumnsProxyModel(QObject* parent)
    : QIdentityProxyModel(parent)
{
}

int StackedColumnsProxyModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0; // don't want to deal with child items.
        //return QAbstractProxyModel::rowCount(parent);
    }
    else
    {
        int count =  sourceModel()->rowCount(QModelIndex()) *
                     sourceModel()->columnCount(QModelIndex());
//        qDebug() << "proxy row count: " << count;
        return count;
    }
}


int StackedColumnsProxyModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 1;
        //return QAbstractProxyModel::columnCount(parent);
    }
    else
    {
        return 1;
    }
}

QModelIndex StackedColumnsProxyModel::index(int row, int column, const QModelIndex& /*parent*/) const
{
    return createIndex(row, column);
}

QModelIndex StackedColumnsProxyModel::parent(const QModelIndex& /* index */) const
{
    return QModelIndex();
}


QModelIndex StackedColumnsProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    // (row, column) -> (column * rowsPerColumn + row, 0)
    auto index = createIndex(
            sourceIndex.column() * sourceModel()->rowCount(QModelIndex())
            + sourceIndex.row(), 0);

//    qDebug() << "mapFromSource " << sourceIndex << " -> " << index;
    return index;
}

QModelIndex StackedColumnsProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    // (column * rowsPerColumn + row) % rowsPerColumn = row
    // (column * rowsPerColumn + row) / rowsPerColumn = column
    int rowCount = sourceModel()->rowCount();
    if (rowCount == 0) rowCount = 1; // avoid division by zero

    auto index = sourceModel()->index(
            proxyIndex.row() % rowCount,
            proxyIndex.row() / rowCount);

//    qDebug() << "mapToSource " << proxyIndex << " -> " << index;

    return index;
}



}}
