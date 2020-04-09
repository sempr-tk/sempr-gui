#include "GeoMapWidget.hpp"

#include <QtQml>
#include <QQuickItem>
#include <QGeoCoordinate>

namespace sempr { namespace gui {

GeoMapWidget::GeoMapWidget(QWidget* parent)
    : QWidget(parent)
{
    form_.setupUi(this);
}

void GeoMapWidget::setup(QAbstractItemModel* model)
{
    // connect the flattener, to make the geometry filter work
    flattenProxy_.setSourceModel(model);
    // connect the geometry filter
    geometryProxy_.setSourceModel(&flattenProxy_);
    // connect the role name proxy
    roleNamesProxy_.setSourceModel(&geometryProxy_);


    // debug: connect a list view
    form_.listView->setModel(&geometryProxy_);

    // make the model known before loading the qml
    //rm_.quickWidget->rootContext()->setContextProperty("circle_model", circleProxy_);
    // load the qml
    //form_.quickWidget->setSource(QUrl::fromLocalFile(":/geomap.qml"));
    form_.quickWidget->setSource(QUrl::fromLocalFile("/home/nils/sempr-gui/ui/geomap.qml"));
}

}}
