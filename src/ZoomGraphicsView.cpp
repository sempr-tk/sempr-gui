#include "ZoomGraphicsView.hpp"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>

namespace sempr { namespace gui {

ZoomGraphicsView::ZoomGraphicsView(QWidget* parent)
    : QGraphicsView(parent), moving_(false)
{
}


void ZoomGraphicsView::wheelEvent(QWheelEvent* event)
{
    const ViewportAnchor anchor = transformationAnchor();
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    int angle = event->angleDelta().y();

    double factor = 1.1;
    if (angle < 0) factor = 1./factor;

    scale(factor, factor);
    setTransformationAnchor(anchor);
}


void ZoomGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::MiddleButton)
    {
        auto point = event->pos();
        auto dx = point.x() - lastPoint_.x();
        auto dy = point.y() - lastPoint_.y();

        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - dx);
        verticalScrollBar()->setValue(verticalScrollBar()->value() - dy);

        lastPoint_ = point;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void ZoomGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::MiddleButton)
    {
        moving_ = true;
        lastPoint_ = event->pos();
    }

    QGraphicsView::mousePressEvent(event);
}

void ZoomGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::MiddleButton))
    {
        moving_ = false;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

}}
