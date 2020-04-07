#ifndef SEMPR_GUI_GEOMAPWIDGET_HPP_
#define SEMPR_GUI_GEOMAPWIDGET_HPP_

#include "../ui/ui_geomap.h"

#include <QStandardItemModel>
#include <QTimer>

#include "RoleNameProxyModel.hpp"

namespace sempr { namespace gui {

class GeoMapWidget : public QWidget {
    Q_OBJECT

    enum Role {
        CoordinateRole = Qt::UserRole + 1000
    };

    Ui_GeoMapWidget form_;

    QStandardItemModel circleModel_;
    RoleNameProxyModel* circleProxy_;

    QTimer debugTimer_;

public:
    GeoMapWidget(QWidget* parent = nullptr);
};

}}

#endif /* include guard: SEMPR_GUI_GEOMAPWIDGET_HPP_ */
