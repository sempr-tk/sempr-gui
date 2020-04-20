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
    QItemSelectionModel* sourceSelectionModel_;

    // a proxy that flattens a tree structure into a list like model
    FlattenTreeProxyModel flattenProxy_;

    // a proxy to give us a view on only the geometries
    // and provide access to the coordinates
    GeometryFilterProxyModel geometryProxy_;

    // a helper proxy that exports methods to convert role names to integers
    // and back, used for qml stuff due to a bug in the qml map view.
    RoleNameProxyModel roleNamesProxy_;

public slots:
    // updates the current (selected) item in the map view
    void onSourceCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous);

    // connected to the qml side, sets the currently selected component to the
    // one that was clicked
    void onGeometryDelegateClicked(int index);
signals:
    // emitted when the source current row changed, contains the matching index
    // for the proxy model in use. To be connected to from the qml side to
    // update the view.
    void currentRowChanged(const QModelIndex& currentProxyIndex);

public:
    GeoMapWidget(QWidget* parent = nullptr);

    /**
        Connects models, filters, and loads the qml.
        Takes as input the basic data model and a selection model that is
        monitored to update the view (highlight current selection).
    */
    void setup(QAbstractItemModel* model, QItemSelectionModel* selectionModel);

};

}}

#endif /* include guard: SEMPR_GUI_GEOMAPWIDGET_HPP_ */
