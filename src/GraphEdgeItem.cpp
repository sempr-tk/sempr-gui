#include "GraphEdgeItem.hpp"
#include <cmath>


#include <QDebug>

namespace sempr { namespace gui {

GraphEdgeItem::GraphEdgeItem(GraphNodeItem* from, GraphNodeItem* to)
    : fromNode_(from), toNode_(to), localHighlight_(false), globalHighlight_(false)
{
    setAcceptedMouseButtons(Qt::NoButton);
    setZValue(-2);

    if (fromNode_) fromNode_->addEdge(this);
    if (toNode_) toNode_->addEdge(this);
}

void GraphEdgeItem::setLocalHighlighted(bool on)
{
    localHighlight_ = on;
    update();
}

void GraphEdgeItem::setGlobalHighlighted(bool on)
{
    globalHighlight_ = on;
    update();
}

GraphNodeItem* GraphEdgeItem::from() const
{
    return fromNode_;
}

GraphNodeItem* GraphEdgeItem::to() const
{
    return toNode_;
}

void GraphEdgeItem::adjust()
{
    if (!fromNode_ || !toNode_) return;

    prepareGeometryChange();

    fromPoint_ = mapFromItem(fromNode_, 0, 0);
    toPoint_ = mapFromItem(toNode_, 0, 0);

    // dont draw from center to center, but from border to border:
    auto vec = toPoint_ - fromPoint_;
    double dist = sqrt( QPointF::dotProduct(vec, vec) );

    if (dist < 10e-5) return;

    double distInsideFrom = fromNode_->innerRadiusInDirection(vec.x(), vec.y());
    double distInsideTo = toNode_->innerRadiusInDirection(-vec.x(), -vec.y());

    auto startPoint = fromPoint_ + vec * distInsideFrom/dist;
    auto endPoint = toPoint_ - vec * distInsideTo/dist;

    fromPoint_ = startPoint;
    toPoint_ = endPoint;

    if (distInsideTo + distInsideFrom > dist) fromPoint_ = toPoint_;
}


QRectF GraphEdgeItem::boundingRect() const
{
    if (!fromNode_ || !toNode_) return QRectF();

    return QRectF(fromPoint_,
                    QSizeF(toPoint_.x() - fromPoint_.x(),
                           toPoint_.y() - fromPoint_.y())).normalized()
                                                          .adjusted(-50, -50, 50, 50);
}


void GraphEdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!fromNode_ || !toNode_) return;

    QColor solBlack(7, 54, 66);
    QColor solWhite(238, 232, 213);
    QColor solBrWhite(253, 246, 227);
    QColor solBlue(38, 139, 210);
    QColor solGreen(133, 153, 0);
    QColor solRed(220, 50, 47);


    painter->setRenderHint(QPainter::RenderHint::HighQualityAntialiasing);

    QPen pen(solBlue, 2);

    if (globalHighlight_)
    {
        pen.setColor(solRed);
    }

    if (localHighlight_)
    {
        pen.setWidth(4);
    }

    painter->setPen(pen);
    painter->drawLine(fromPoint_, toPoint_);

    // draw an arrow head
    auto vec = toPoint_ - fromPoint_;
    vec = vec / sqrt( QPointF::dotProduct(vec, vec) ); // normalize

    auto rotate =
        [](const QPointF& vec, double alpha) -> QPointF
        {
            QPointF rot1( cos(alpha), -sin(alpha) );
            QPointF rot2( sin(alpha),  cos(alpha) );
            return QPointF(QPointF::dotProduct(rot1, vec),
                           QPointF::dotProduct(rot2, vec));
        };

    QPointF r1 = rotate(vec, M_PI - M_PI_4/2.);
    QPointF r2 = rotate(vec, M_PI + M_PI_4/2.);

    auto p1 = toPoint_ + 10. * r1;
    auto p2 = toPoint_ + 10. * r2;

    QPolygonF poly;
    poly.append(toPoint_);
    poly.append(p1);
    poly.append(p2);

    painter->setBrush( pen.brush() );
    painter->drawPolygon(poly);
}

}}
