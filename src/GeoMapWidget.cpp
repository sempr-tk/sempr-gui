#include "GeoMapWidget.hpp"
#include "../ui/ui_geomap.h"

#include <QtQml>
#include <QQuickItem>
#include <QGeoCoordinate>

namespace sempr { namespace gui {

GeoMapWidget::GeoMapWidget(QWidget* parent)
    : QWidget(parent), form_(new Ui::GeoMapWidget)
{
    form_->setupUi(this);
}

GeoMapWidget::~GeoMapWidget()
{
    delete form_;
}

void GeoMapWidget::setup(QAbstractItemModel* model, QItemSelectionModel* sourceSelectionModel)
{
    // connect the flattener, to make the geometry filter work
    flattenProxy_.setSourceModel(model);
    // connect the geometry filter
    geometryProxy_.setSourceModel(&flattenProxy_);
    // connect the role name proxy
    roleNamesProxy_.setSourceModel(&geometryProxy_);

    // make the model known before loading the qml
    form_->quickWidget->rootContext()->setContextProperty("geometryModel", &roleNamesProxy_);
    // also, make *this* known, in oder to connect to signals
    form_->quickWidget->rootContext()->setContextProperty("GeoMapWidget", this);

    // load the qml
    //form_.quickWidget->setSource(QUrl::fromLocalFile(":/geomap.qml"));
    form_->quickWidget->setSource(QUrl::fromLocalFile("/home/nils/sempr-gui/ui/geomap.qml"));


    // save the selection model, connect to its currentRowChanged signal to
    // mirror the current item
    sourceSelectionModel_ = sourceSelectionModel;
    connect(sourceSelectionModel_, &QItemSelectionModel::currentRowChanged,
            this, &GeoMapWidget::onSourceCurrentRowChanged);

    QObject* rootObject = form_->quickWidget->rootObject();
    connect(rootObject, SIGNAL(geometryDelegateClicked(int)),
            this, SLOT(onGeometryDelegateClicked(int)));
}


void GeoMapWidget::onGeometryDelegateClicked(int index)
{
    // map from the index in the map view to the index in the source model
    auto proxyIndex = roleNamesProxy_.index(index, 0);
    auto sourceIndex =
        flattenProxy_.mapToSource(
            geometryProxy_.mapToSource(
                roleNamesProxy_.mapToSource(
                    proxyIndex
                )
            )
        );

    // update the source selection model
    sourceSelectionModel_->setCurrentIndex(sourceIndex, QItemSelectionModel::SelectCurrent);
}

void GeoMapWidget::onSourceCurrentRowChanged(
        const QModelIndex& current,
        const QModelIndex& /*previous*/)
{
    auto proxyIndex =
        roleNamesProxy_.mapFromSource(
            geometryProxy_.mapFromSource(
                flattenProxy_.mapFromSource(
                    current
                )
            )
        );

    emit currentRowChanged(proxyIndex);
}

}}
