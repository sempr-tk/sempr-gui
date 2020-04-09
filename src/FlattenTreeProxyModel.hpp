#ifndef SEMPR_GUI_FLATTENTREEPROXYMODEL_HPP_
#define SEMPR_GUI_FLATTENTREEPROXYMODEL_HPP_

#include <QAbstractProxyModel>
#include <QList>

namespace sempr { namespace gui {

/**
    This proxy flattens any tree model into a list. For this to work you
    need a constant column count, which is assumed to be 1.

    Internally, this model tracks where the parent of an item in the source
    model is in the list of this model as its (index+1). +1 to reserve 0 as an
    indicator that the parent of the item is the root.

    source model            this model      internal data
    root                    root            root
     L P1                    L P1            L [0]: 0
       L C1                  L C1            L [1]: 1
       L C2                  L C2            L [2]: 1
         L C21       ->      L C21      ->   L [3]: 3
         L C22               L C22           L [4]: 3
     L P2                    L P2            L [5]: 0
     L P3                    L P3            L [6]: 0
       L C3                  L C3            L [7]: 7
       L C4                  L C4            L [8]: 7

    Limitations: This proxy...
        - Assumes a column count of 1
        - Forwards dataChanged signals
        - Should handle row-insertions and row-removals
        - Was only tested with removal and insertion of one row at a time
        - Will *not* handle moving rows
        - Does not connect to layoutChanged

*/
class FlattenTreeProxyModel : public QAbstractProxyModel {
    // the list indicating who's whose parent
    QList<int> parents_;

    // iterates the source model to fill the parents_ list
    void rebuild();

    // recursive method used in rebuild to fill the parents_ list
    void addChildrenToList(const QModelIndex& index);

    // count all decendants of the given sourceIndex
    int numDecendants(const QModelIndex& sourceIndex) const;

    // first and last row to remove need to be computed in
    // "about to be removed", but must only be removed when they are actually
    // removed from the source, in "rowsRemoved".
    int aboutToRemoveFirstRow_, aboutToRemoveLastRow_;

private slots:
    // we need to map the indices and forward the data changed signal..
    // I really would've thought that the QAbstractProxyModel already does this.
    void onSourceDataChanged(const QModelIndex& tl, const QModelIndex& br,
                             const QVector<int>& roles = QVector<int>());

    // I don't care about columns changing.
    // Or about header data.
    // Also, our source model will not do any real layout changes. Right?
    // But modelReset is relevant!
    void onSourceModelReset();
    // also, when rows are inserted, removed moved, we need to know.
    void onSourceRowsAboutToBeInserted(const QModelIndex& sourceParent, int first, int last);
    void onSourceRowsInserted(const QModelIndex& sourceParent, int first, int last);
    void onSourceRowsAboutToBeRemoved(const QModelIndex& sourceParent, int first, int last);
    void onSourceRowsRemoved(const QModelIndex& sourceParent, int first, int last);

public:
    FlattenTreeProxyModel(QObject* parent = nullptr);

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    void setSourceModel(QAbstractItemModel* model) override;
};


}}

#endif /* include guard: SEMPR_GUI_FLATTENTREEPROXYMODEL_HPP_ */
