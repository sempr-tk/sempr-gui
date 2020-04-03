#ifndef SEMPR_GUI_GEOMAPWIDGET_HPP_
#define SEMPR_GUI_GEOMAPWIDGET_HPP_

#include "../ui/ui_geomap.h"

namespace sempr { namespace gui {

class GeoMapWidget : public QWidget {
    Q_OBJECT

    Ui_GeoMapWidget form_;
public:
    GeoMapWidget(QWidget* parent = nullptr);
};

}}

#endif /* include guard: SEMPR_GUI_GEOMAPWIDGET_HPP_ */
