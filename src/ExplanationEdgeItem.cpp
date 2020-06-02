#include "ExplanationEdgeItem.hpp"

namespace sempr { namespace gui {

ExplanationEdgeItem::ExplanationEdgeItem(ExplanationNodeItem* from, ExplanationNodeItem* to)
    : fromNode_(from), toNode_(to)
{
    setAcceptedMouseButtons(Qt::NoButton);
    setZValue(-1);

    if (fromNode_) fromNode_->addEdge(this);
    if (toNode_) toNode_->addEdge(this);
}

ExplanationNodeItem* ExplanationEdgeItem::from() const
{
    return fromNode_;
}

ExplanationNodeItem* ExplanationEdgeItem::to() const
{
    return toNode_;
}

void ExplanationEdgeItem::adjust()
{
    if (!fromNode_ || !toNode_) return;

    prepareGeometryChange();
    fromPoint_ = mapFromItem(fromNode_, 0, 0);
    toPoint_ = mapFromItem(toNode_, 0, 0);
}

QRectF ExplanationEdgeItem::boundingRect() const
{
    if (!fromNode_ || !toNode_) return QRectF();

    return QRectF(fromPoint_,
                    QSizeF(toPoint_.x() - fromPoint_.x(),
                           toPoint_.y() - fromPoint_.y())).normalized();

}

void ExplanationEdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!fromNode_ || !toNode_) return;
    painter->setRenderHint(QPainter::RenderHint::HighQualityAntialiasing);

    QColor solBlack(7, 54, 66);
    QColor solWhite(238, 232, 213);
    QColor solBrWhite(253, 246, 227);
    QColor solBlue(38, 139, 210);
    QColor solGreen(133, 153, 0);
    QColor solRed(220, 50, 47);

    QPen pen(solBlue, 2);
    painter->setPen(pen);
    painter->drawLine(fromPoint_, toPoint_);
}


}}
