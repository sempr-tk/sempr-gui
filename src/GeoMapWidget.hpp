#ifndef SEMPR_GUI_GEOMAPWIDGET_HPP_
#define SEMPR_GUI_GEOMAPWIDGET_HPP_

#include <QWidget>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QGeoCoordinate>

#include <QTimer>

#include "RoleNameProxyModel.hpp"
#include "GeometryFilterProxyModel.hpp"
#include "FlattenTreeProxyModel.hpp"

namespace Ui {
    class GeoMapWidget;
}

namespace sempr { namespace gui {

class GeoMapWidget : public QWidget {
    Q_OBJECT

    Ui::GeoMapWidget* form_;

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
    ~GeoMapWidget();

    /**
        Connects models, filters, and loads the qml.
        Takes as input the basic data model and a selection model that is
        monitored to update the view (highlight current selection).
    */
    void setup(QAbstractItemModel* model, QItemSelectionModel* selectionModel);

    // returns the center of the currently visible map section
    QGeoCoordinate mapCenter() const;

    // returns the coordinate of the top-left corner of the map
    QGeoCoordinate mapTopLeft() const;

    // returns the coordinate of the bottom-right corner of the map
    QGeoCoordinate mapBottomRight() const;
};

}}

#endif /* include guard: SEMPR_GUI_GEOMAPWIDGET_HPP_ */
