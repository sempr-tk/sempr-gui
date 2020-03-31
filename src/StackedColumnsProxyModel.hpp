#ifndef SEMPR_GUI_STACKEDCOLUMNSPROXYMODEL_HPP_
#define SEMPR_GUI_STACKEDCOLUMNSPROXYMODEL_HPP_

#include <QIdentityProxyModel>

namespace sempr { namespace gui {

/**
    This proxy model transforms a given model with multiple columns into a
    single-column model. Columns are virtually stacked by a simple
    transformation of model indices.

    Use this only with flat models that have only one level of depth, i.e.
    parent() for any model index always returns an invalid QModelIndex().
*/
class StackedColumnsProxyModel : public QIdentityProxyModel {
public:
    StackedColumnsProxyModel(QObject* parent = nullptr);

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
};


}}

#endif /* include guard: SEMPR_GUI_STACKEDCOLUMNSPROXYMODEL_HPP_ */
