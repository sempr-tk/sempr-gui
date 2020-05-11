#include "ReteEdgeItem.hpp"

namespace sempr { namespace gui {

ReteEdgeItem::ReteEdgeItem(ReteNodeItem* from, ReteNodeItem* to)
    : fromNode_(from), toNode_(to)
{
    setAcceptedMouseButtons(Qt::NoButton);

    if (fromNode_) fromNode_->addEdge(this);
    if (toNode_) toNode_->addEdge(this);
}

void ReteEdgeItem::adjust()
{
    if (!fromNode_ || !toNode_) return;

    prepareGeometryChange();

    fromPoint_ = mapFromItem(fromNode_, 0, 0);
    toPoint_ = mapFromItem(toNode_, 0, 0);
}


QRectF ReteEdgeItem::boundingRect() const
{
    if (!fromNode_ || !toNode_) return QRectF();

    return QRectF(fromPoint_,
                    QSizeF(toPoint_.x() - fromPoint_.x(),
                           toPoint_.y() - fromPoint_.y())).normalized();
}


void ReteEdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!fromNode_ || !toNode_) return;

    QPen pen(Qt::black, 1);
    painter->drawLine(fromPoint_, toPoint_);
}

}}
