#ifndef SEMPR_GUI_UNIQUEFILTERPROXYMODEL_HPP_
#define SEMPR_GUI_UNIQUEFILTERPROXYMODEL_HPP_

#include <QSortFilterProxyModel>

namespace sempr { namespace gui {

/**
    This proxy removes duplicate rows based on the first column.
*/
class UniqueFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

    /**
        This map stores a mapping of a value to the firsrt index at which it
        was found in the source model. This allows us to check in
        filterAcceptRow if the index is in there, or if it is a duplicate.
    */
    QMap<QVariant, QPersistentModelIndex> valueToIndex_;

protected slots:
    // rebuilds the valueToIndex-map
    void buildMap();

public:
    UniqueFilterProxyModel(QObject* parent = nullptr);

    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

    void setSourceModel(QAbstractItemModel* source) override;

};


}}

#endif /* include guard: SEMPR_GUI_UNIQUEFILTERPROXYMODEL_HPP_ */
