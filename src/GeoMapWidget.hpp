#ifndef SEMPR_GUI_GEOMAPWIDGET_HPP_
#define SEMPR_GUI_GEOMAPWIDGET_HPP_

#include "../ui/ui_geomap.h"

#include <QStandardItemModel>
#include <QAbstractItemModel>

#include <QTimer>

#include "RoleNameProxyModel.hpp"
#include "GeometryFilterProxyModel.hpp"
#include "FlattenTreeProxyModel.hpp"

namespace sempr { namespace gui {

class GeoMapWidget : public QWidget {
    Q_OBJECT

    Ui_GeoMapWidget form_;

    // the data model to connect to
    QAbstractItemModel* model_;

    // a proxy that flattens a tree structure into a list like model
    FlattenTreeProxyModel flattenProxy_;

    // a proxy to give us a view on only the geometries
    // and provide access to the coordinates
    GeometryFilterProxyModel geometryProxy_;

    // a helper proxy that exports methods to convert role names to integers
    // and back, used for qml stuff due to a bug in the qml map view.
    RoleNameProxyModel roleNamesProxy_;

public:
    GeoMapWidget(QWidget* parent = nullptr);

    /**
        Connects models, filters, and loads the qml.
    */
    void setup(QAbstractItemModel* model);

};

}}

#endif /* include guard: SEMPR_GUI_GEOMAPWIDGET_HPP_ */
