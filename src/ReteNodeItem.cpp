#include "ReteNodeItem.hpp"
#include <QFontMetrics>
#include <QApplication>
#include "ReteEdgeItem.hpp"

#include <iostream>

namespace sempr { namespace gui {

ReteNodeItem::ReteNodeItem(const QString& text)
    : text_(text)
{
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsMovable);
    setZValue(-1);
}

QRectF ReteNodeItem::boundingRect() const
{
    auto fm = QApplication::fontMetrics();
    return fm.boundingRect(text_).adjusted(-20, -20, 20, 20);
}

void ReteNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    auto rect = boundingRect();
    QPen pen(Qt::black, 3);
    painter->setPen(pen);
    painter->drawRect(rect);
    painter->drawText(rect, Qt::AlignCenter, text_);
}

void ReteNodeItem::addEdge(ReteEdgeItem* edge)
{
    edges_.push_back(edge);
}


QVariant ReteNodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged)
    {
        for (auto edge : edges_)
        {
            edge->adjust();
            edge->update();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}


}}
