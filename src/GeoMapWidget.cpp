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
    roles[Role::CoordinateRole] = QByteArray("coordinate_foo");
    roles[Qt::DisplayRole] = QByteArray("display");
    circleModel_.setItemRoleNames(roles);

    // expose role names to qml
    circleProxy_ = new RoleNameProxyModel();
    circleProxy_->setSourceModel(&circleModel_);

    // debug: connect a list view
    form_.listView->setModel(circleProxy_);

    // make the model known before loading the qml
    form_.quickWidget->rootContext()->setContextProperty("circle_model", circleProxy_);
    // load the qml
    //form_.quickWidget->setSource(QUrl::fromLocalFile(":/geomap.qml"));
    form_.quickWidget->setSource(QUrl::fromLocalFile("/home/nils/sempr-gui/ui/geomap.qml"));

    // add some data
    QStandardItem* item = new QStandardItem();
    item->setData(QVariant::fromValue(QGeoCoordinate(52.28, 8.051)),
                  Role::CoordinateRole);
    item->setData("foooooo", Qt::DisplayRole);
    item->setEditable(true);
    circleModel_.appendRow(item);

    /*
    QStandardItem* item2 = new QStandardItem();
    item2->setData(QVariant::fromValue(QGeoCoordinate(52.28, 8.03)),
                  Role::CoordinateRole);
    circleModel_.appendRow(item2);
    */


    connect(&debugTimer_, &QTimer::timeout,
            this,
            [this]()
            {
                auto index = this->circleModel_.index(0, 0);
                auto coord = index.data(Role::CoordinateRole).value<QGeoCoordinate>();
                auto display = index.data(Qt::DisplayRole).value<QString>();
                qDebug() << display << " -- " << coord;
            });
    debugTimer_.start(1000);
}

}}
