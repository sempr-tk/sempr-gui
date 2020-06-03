#include "GraphEdgeItem.hpp"

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
}


QRectF GraphEdgeItem::boundingRect() const
{
    if (!fromNode_ || !toNode_) return QRectF();

    return QRectF(fromPoint_,
                    QSizeF(toPoint_.x() - fromPoint_.x(),
                           toPoint_.y() - fromPoint_.y())).normalized();
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
}

}}
