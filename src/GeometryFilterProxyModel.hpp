#ifndef SEMPR_GUI_GEOMETRYFILTERPROXYMODEL_HPP_
#define SEMPR_GUI_GEOMETRYFILTERPROXYMODEL_HPP_

#include <QSortFilterProxyModel>
#include "ECModel.hpp"

#include <sempr/component/GeosGeometry.hpp>

namespace sempr { namespace gui {

/**
    This model filters the components, accepting only those who are a
    GeoGeometry, and adds roles to get and set the geometries coordinates.
*/
class GeometryFilterProxyModel : public QSortFilterProxyModel {
protected:
    // helper - retrieve the component ptr through the ComponentPtrRole and
    // cast it to a GeosGeometry::Ptr
    static std::shared_ptr<GeosGeometryInterface> geomPointerFromIndex(const QModelIndex& index);

public:
    GeometryFilterProxyModel(QObject* parent = nullptr);

    /**
        Returns true if the component at the given entry is a GeosGeometry.
    */
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

    /**
        Enables the role for coordinates in qml
    */
    QHash<int, QByteArray> roleNames() const override;

    /**
        Provides access to the coordinates of the geometry, and a description
        of its type.
    */
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};

}}

#endif /* include guard: SEMPR_GUI_GEOMETRYFILTERPROXYMODEL_HPP_ */
