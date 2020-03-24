#ifndef SEMPR_GUI_ECMODEL_HPP_
#define SEMPR_GUI_ECMODEL_HPP_

#include <QAbstractItemModel>
#include <vector>
#include <map>
#include <string>

#include "ModelEntry.hpp"

namespace sempr { namespace gui {

/**
    The ECModel is an implementation of the QAbstractItemModel and provides
    access to the Entity-Component pairs stored in ModelEntry structs.
    The data is organized in a shallow tree structure, where the first level
    entries point to the goup defined by the entity id and the second level to
    the component / the ModelEntry belonging to the entity.

    The first level contains the entity ids, the second level the component
    type. There is **no** editing provided directly through this model --
    specialized views are to be used that work on the Qt::UserRole data, which
    points to the ModelEntry!
*/
class ECModel : public QAbstractItemModel {
public:
    typedef std::vector<ModelEntry> ModelEntryGroup;

    std::vector<ModelEntryGroup> data_;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QModelIndex index(int row, int col, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& index) const override;
};


}}

#endif /* include guard: SEMPR_GUI_ECMODEL_HPP_ */
