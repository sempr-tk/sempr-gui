#ifndef SEMPR_GUI_ANYCOLUMNFILTERPROXYMODEL_HPP_
#define SEMPR_GUI_ANYCOLUMNFILTERPROXYMODEL_HPP_

#include <QSortFilterProxyModel>

namespace sempr { namespace gui {

/**
    This filter model ignores the filterKeyColumn and filterRole and instead
    applies the given regexp to the concatenation of all columns DisplayRole
    data, with a space inbetween.
*/
class AnyColumnFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
};

}}

#endif /* include guard: SEMPR_GUI_ANYCOLUMNFILTERPROXYMODEL_HPP_ */
