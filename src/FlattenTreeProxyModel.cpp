#include "FlattenTreeProxyModel.hpp"

#include <QtCore>

namespace sempr { namespace gui {

FlattenTreeProxyModel::FlattenTreeProxyModel(QObject* parent)
    : QAbstractProxyModel(parent)
{
}

void FlattenTreeProxyModel::rebuild()
{
    parents_.clear();

    auto model = sourceModel();
    if (!model) return;

    addChildrenToList(QModelIndex());
}


void FlattenTreeProxyModel::setSourceModel(QAbstractItemModel* model)
{
    this->beginResetModel();
    // disconnect from previous model
    disconnect(model, &QAbstractItemModel::dataChanged,
            this, &FlattenTreeProxyModel::onSourceDataChanged);
    disconnect(model, &QAbstractItemModel::modelReset,
            this, &FlattenTreeProxyModel::onSourceModelReset);

    // insertion of rows
    disconnect(model, &QAbstractItemModel::rowsAboutToBeInserted,
            this, &FlattenTreeProxyModel::onSourceRowsAboutToBeInserted);
    disconnect(model, &QAbstractItemModel::rowsInserted,
            this, &FlattenTreeProxyModel::onSourceRowsInserted);

    // removal of rows
    disconnect(model, &QAbstractItemModel::rowsAboutToBeRemoved,
            this, &FlattenTreeProxyModel::onSourceRowsAboutToBeRemoved);
    disconnect(model, &QAbstractItemModel::rowsRemoved,
            this, &FlattenTreeProxyModel::onSourceRowsRemoved);

    // connect to new model
    if (model)
    {
        // data changed and model reset
        connect(model, &QAbstractItemModel::dataChanged,
                this, &FlattenTreeProxyModel::onSourceDataChanged);
        connect(model, &QAbstractItemModel::modelReset,
                this, &FlattenTreeProxyModel::onSourceModelReset);

        // insertion of rows
        connect(model, &QAbstractItemModel::rowsAboutToBeInserted,
                this, &FlattenTreeProxyModel::onSourceRowsAboutToBeInserted);
        connect(model, &QAbstractItemModel::rowsInserted,
                this, &FlattenTreeProxyModel::onSourceRowsInserted);

        // removal of rows
        connect(model, &QAbstractItemModel::rowsAboutToBeRemoved,
                this, &FlattenTreeProxyModel::onSourceRowsAboutToBeRemoved);
        connect(model, &QAbstractItemModel::rowsRemoved,
                this, &FlattenTreeProxyModel::onSourceRowsRemoved);
    }

    QAbstractProxyModel::setSourceModel(model);

    rebuild();
    this->endResetModel();
}


/*
    This method is called recursively to fill the parents_ list.
    It is always directly called for the last item that was added to it, so
    the direct parent of the children added is the last one added.
    Hence, the parent-index is parents_.size()-1, and since we add +1 in order
    to use "0" for the root item, we just store parents_.size().
*/
void FlattenTreeProxyModel::addChildrenToList(const QModelIndex& parentIndex)
{
    int numRows = sourceModel()->rowCount(parentIndex);
    int parent = parents_.size();

    for (int i = 0; i < numRows; i++)
    {
        auto childIndex = sourceModel()->index(i, 0, parentIndex);
        // add the child
        parents_.append(parent);
        // and directly add the grandchildren afterwards
        addChildrenToList(childIndex);
    }
}


int FlattenTreeProxyModel::numDecendants(const QModelIndex& sourceIndex) const
{
    int rowCount = sourceModel()->rowCount(sourceIndex);
    // the number of descendants of this index is the number of its children,
    // plus the number of its childrens decendants.
    int num = rowCount;
    for (int i = 0; i < rowCount; i++)
    {
        auto child = sourceModel()->index(i, 0, sourceIndex);
        // recursive!
        num += numDecendants(child);
    }

    return num;
}


QModelIndex FlattenTreeProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    // invalid source -> invalid proxy
    if (!sourceIndex.isValid()) return QModelIndex();

    // Basically, we need to count all indices before this.
    // To do this, we traverse the tree again recursively: Assume we know the
    // proxy-index of the parent of this (sourceIndex) index. All we need to do
    // is add the number of siblings above this, and the number of all their
    // decendants -- this we can do with numDecendants.
    int numSiblingsAbove = sourceIndex.row();
    int numDecendantsOfSiblingsAbove = 0;
    for (int i = 0; i < numSiblingsAbove; i++)
    {
        numDecendantsOfSiblingsAbove += numDecendants(sourceIndex.sibling(i, 0));
    }

    auto parentsProxyIndex = mapFromSource(sourceIndex.parent());
    int parentsProxyRow = (parentsProxyIndex.isValid() ? parentsProxyIndex.row() : -1);

    auto proxyIndex = this->index(parentsProxyRow +
                       numSiblingsAbove +
                       numDecendantsOfSiblingsAbove + 1, 0);

//    qDebug() << "source -> proxy: " << sourceIndex << " -> " << proxyIndex;
    return proxyIndex;
}

QModelIndex FlattenTreeProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if (!proxyIndex.isValid()) return QModelIndex();

    // in order to create really valid source indices, we need to traverse
    // the source model again, from root to child. But to figure out where to
    // look we need to go the other way round, by checking in the internal data
    // where the proxy-parent is, and compute the source-index for that.
    int parentsId = parents_[proxyIndex.row()];
    int rowInParent = 0;
    // to compute which direct child of its parent this one is, we need to count
    // how many other items are a direct child of it and above this entry.
    for (int i = parentsId; i < proxyIndex.row(); i++)
    {
        if (parents_[i] == parentsId) rowInParent++;
    }

    // now, if the parentsId is 0 it is the root item, and we can compute the
    // index directly
    if (parentsId == 0) return sourceModel()->index(rowInParent, 0, QModelIndex());
    // else we just call mapToSource for the parent, recursively.
    // (subtract 1 from the parentsId to get its index in the parents_ list!)
    auto sourceParent = mapToSource(this->index(parentsId-1, 0));
    auto sourceIndex = sourceModel()->index(rowInParent, 0, sourceParent);

//    qDebug() << "proxy -> source: " << proxyIndex << " -> " << sourceIndex;
    return sourceIndex;
}

int FlattenTreeProxyModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

int FlattenTreeProxyModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return parents_.size();
}

QModelIndex FlattenTreeProxyModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid()) return QModelIndex();

    return this->createIndex(row, column);
}

QModelIndex FlattenTreeProxyModel::parent(const QModelIndex& /*index*/) const
{
    return QModelIndex();
}


void FlattenTreeProxyModel::onSourceDataChanged(
        const QModelIndex& tl,
        const QModelIndex& br,
        const QVector<int>& roles)
{
    // for every affected row, map it to the proxy index,
    // and emit a dataChanged.
    for (int sourceRow = tl.row(); sourceRow <= br.row(); sourceRow++)
    {
        auto proxyTL = mapFromSource(tl.sibling(sourceRow, tl.column()));
        auto proxyBR = proxyTL.sibling(proxyTL.row(), br.column());

        emit dataChanged(proxyTL, proxyBR, roles);
//        qDebug() << "FlattenTreeProxyModel onSourceDataChanged" << proxyTL << proxyBR << roles;
    }
}

void FlattenTreeProxyModel::onSourceModelReset()
{
    beginResetModel();
    rebuild();
    endResetModel();
}


void FlattenTreeProxyModel::onSourceRowsAboutToBeInserted(
        const QModelIndex& sourceParent,
        int first, int last)
{
//    qDebug() << "before insert " << parents_;

    // find the position of the first source item in the proxy
    auto sourceFirst = sourceModel()->index(first, 0, sourceParent);
    auto proxyFirst = mapFromSource(sourceFirst);
    // signal begin inserting
    beginInsertRows(QModelIndex(), proxyFirst.row(), proxyFirst.row() + (last-first));
}

void FlattenTreeProxyModel::onSourceRowsInserted(
        const QModelIndex& sourceParent,
        int first, int last)
{
    // find the position of the first source item in the proxy
    auto sourceFirst = sourceModel()->index(first, 0, sourceParent);
    auto proxyFirst = mapFromSource(sourceFirst);

    // beginInsertRows should have been called in onSourceRowsAboutToBeInserted

    // there will be items inserted after proxyFirst, so all parent-indices
    // below that which are greater or equal to proxyFirst must be incremented
    // by (last-first+1). (Remember: the parents_ list holds parents rows+1)
    for (int i = proxyFirst.row()+1; i < parents_.size(); i++)
    {
        if (parents_[i] >= (proxyFirst.row()+1)) parents_[i] += (last-first)+1;
    }

    // also, insert more rows which all have the same parent
    auto proxyParent = mapFromSource(sourceParent);
    int proxyParentId = (proxyParent.isValid() ? proxyParent.row()+1 : 0);
    for (int i = first; i <= last; i++)
    {
        parents_.insert(proxyFirst.row(), proxyParentId);
    }

    endInsertRows();

    // One more thing: The items inserted may have sub-items, that we totally
    // missed so far! So, process them recursively, treat every inserted item
    // as a parent.
    for (int i = first; i <= last; i++)
    {
        auto sourceIndex = sourceModel()->index(first, 0, sourceParent);
        if (!sourceIndex.isValid()) continue;

        int childCount = sourceModel()->rowCount(sourceIndex);
        if (childCount > 0)
        {
            onSourceRowsAboutToBeInserted(sourceIndex, 0, childCount-1);
            onSourceRowsInserted(sourceIndex, 0, childCount-1);
        }
    }

//    qDebug() << "after insert " << parents_;
}


void FlattenTreeProxyModel::onSourceRowsAboutToBeRemoved(
        const QModelIndex& sourceParent,
        int first, int last)
{
//    qDebug() << "before remove " << parents_;

    // in order to get valid indices here we do all this in the
    // *AboutToBe*-removed method!
    auto sourceFirst = sourceModel()->index(first, 0, sourceParent);
    auto sourceLast = sourceModel()->index(last, 0, sourceParent);

    // the first entry in the proxy to remove is the corresponding entry of
    // sourceFirst:
    auto proxyFirst = mapFromSource(sourceFirst);
    auto proxyLastToplevel = mapFromSource(sourceLast);
    // but the last entry in the proxy to remove is the last child of
    // sourceLast! Even worse: The last descendant of sourceLast!
    // How to find it? Just iterate the parents_ list. As soon as an entry is
    // found that has a parent that is before proxyFirst, we found the first
    // entry *not* to remove, as it opens a new subtree.
    int proxyLastRow = proxyLastToplevel.row();
    while (parents_.size() > (proxyLastRow+1) &&
           parents_[proxyLastRow+1] >= (proxyFirst.row()+1))
    {
        // as long as there is a next entry
        //      (parents_.size() > (proxyLastRow+1))
        // and the entries parent is the first to remove or any after that,
        //      (parents_[proxyLastRow+1] >= (proxyFirst.row()+1)
        // step forward, the next is still to be removed:
        proxyLastRow++;
        // else we reached the first item not unter the tree of to-be-removed
        // ones.
    }

    beginRemoveRows(QModelIndex(), proxyFirst.row(), proxyLastRow);

    // remember to do the actual removal in onSourceRowsRemoved
    aboutToRemoveFirstRow_ = proxyFirst.row();
    aboutToRemoveLastRow_ = proxyLastRow;
}

void FlattenTreeProxyModel::onSourceRowsRemoved(
        const QModelIndex& /*sourceParent*/,
        int /*first*/, int /*last*/)
{
    // ignore input, we assume that this matches the last call to
    // rowsAboutToBeRemoved

    // now, what does removal mean? Removing list entries and decrementing
    // the parent-ids of all entries below the last to be removed, if they
    // refer to a parent id that comes after those who are removed.
    int numRemoved = aboutToRemoveLastRow_ - aboutToRemoveFirstRow_ + 1;

    // decrement before removing.
    for (int i = aboutToRemoveLastRow_ + 1; i < parents_.size(); i++)
    {
        if (parents_[i] >= (aboutToRemoveLastRow_+1)) parents_[i] -= numRemoved;
    }

    // then remove
    parents_.erase(parents_.begin() + aboutToRemoveFirstRow_,
                   parents_.begin() + aboutToRemoveLastRow_+1);


    endRemoveRows();

//    qDebug() << "after remove " << parents_;
}


}}
