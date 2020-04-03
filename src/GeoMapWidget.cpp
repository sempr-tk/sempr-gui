#include "GeoMapWidget.hpp"

#include <QtQml>
#include <QQuickItem>
#include <QGeoCoordinate>

namespace sempr { namespace gui {

GeoMapWidget::GeoMapWidget(QWidget* parent)
    : QWidget(parent)
{
    form_.setupUi(this);

    // create a model that will serve the qml mapview
    QHash<int, QByteArray> roles;
    roles[Role::CoordinateRole] = QByteArray("coordinate");
    circleModel_.setItemRoleNames(roles);

    // make the model known before loading the qml
    form_.quickWidget->rootContext()->setContextProperty("circle_model", &circleModel_);
    // load the qml
    form_.quickWidget->setSource(QUrl::fromLocalFile(":/geomap.qml"));

    // add some data
    QStandardItem* item = new QStandardItem();
    item->setData(QVariant::fromValue(QGeoCoordinate(52.283, 8.050)),
                  Role::CoordinateRole);
    circleModel_.appendRow(item);
}

}}
