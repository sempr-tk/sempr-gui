#ifndef SEMPR_GUI_CUSTOMDATAROLES_HPP_
#define SEMPR_GUI_CUSTOMDATAROLES_HPP_

#include <QAbstractItemModel>

namespace sempr { namespace gui {

/**
  One global definition of custom item data roles for this gui, in order to
  not mix them up.
  */
enum Role {
    // provided by the ECModel itself:
    EntityIdRole = Qt::UserRole, // ro: EntityId from ModelEntry
    ComponentIdRole,             // ro: ComponentId from ModelEntry
    ComponentJsonRole,           // rw: json representation of component
    ComponentPtrRole,            // rw: component pointer
    ComponentMutableRole,        // ro: bool isComponentMutable
    ModelEntryRole,              // ro: whole ModelEntry
    // provided by the GeometryFilterProxyModel:
    CoordinatesRole,             // rw: coordinates of the geos::geom::Geometry
    GeosGeometryTypeRole,        // ro: the type of the GeosGeometry
    LastRole
};



}}

#endif /* include guard: SEMPR_GUI_CUSTOMDATAROLES_HPP_ */
