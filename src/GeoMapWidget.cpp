#include "GeoMapWidget.hpp"

namespace sempr { namespace gui {

GeoMapWidget::GeoMapWidget(QWidget* parent)
    : QWidget(parent)
{
    form_.setupUi(this);
    form_.quickWidget->setSource(QUrl::fromLocalFile(":/geomap.qml"));
}

}}
