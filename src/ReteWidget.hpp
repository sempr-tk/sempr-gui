#ifndef SEMPR_GUI_RETEWIDGET_HPP_
#define SEMPR_GUI_RETEWIDGET_HPP_

#include <QtWidgets>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>

#include <map>

#include "AbstractInterface.hpp"

namespace Ui {
    class ReteWidget;
}


namespace sempr { namespace gui {

/**
    A widget with a QGraphicsView to display the rete network
*/
class ReteWidget : public QWidget {
    Q_OBJECT

    Ui::ReteWidget* form_;

    QGraphicsScene scene_;
    AbstractInterface::Ptr sempr_;

    // a list of graphics items that were added to the scene
    std::map<std::string, QGraphicsItem*> items_;

    /**
        Requests the network information from the sempr core and re-builds the
        visual representation the in graphics scene
    */
    void rebuild();

public:
    ReteWidget(QWidget* parent = nullptr);
    ~ReteWidget();

    /**
        Initializes the connection to sempr
    */
    void setConnection(AbstractInterface::Ptr);
};

}}

#endif /* include guard: SEMPR_GUI_RETEWIDGET_HPP_ */
