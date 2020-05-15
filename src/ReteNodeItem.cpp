#include "ReteNodeItem.hpp"
#include <QFontMetrics>
#include <QApplication>
#include "ReteEdgeItem.hpp"

#include <iostream>

namespace sempr { namespace gui {

ReteNodeItem::ReteNodeItem(const QString& text)
    : text_(text), highlight_(false), localHighlight_(false)
{
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsMovable);
    setZValue(-1);
}

void ReteNodeItem::setHighlighted(bool on)
{
    highlight_ = on;
    update();
}

bool ReteNodeItem::isHighlighted() const
{
    return highlight_;
}

QRectF ReteNodeItem::boundingRect() const
{
    auto fm = QApplication::fontMetrics();
    auto rect = fm.boundingRect(text_);
    rect.moveCenter({0, 0});
    rect.adjust(-20, -20, 20, 20);
    return rect;
}

void ReteNodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* e)
{
    localHighlight_ = true;
    update();

    for (auto e : edges_)
    {
        e->setLocalHighlighted(true);
    }

    QGraphicsItem::hoverEnterEvent(e);
}

void ReteNodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* e)
{
    localHighlight_ = false;
    update();

    for (auto e : edges_)
    {
        e->setLocalHighlighted(false);
    }

    QGraphicsItem::hoverLeaveEvent(e);
}

void ReteNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    auto rect = boundingRect();
    rect.adjust(5, 5, -5, -5); // shrink a bit to have everything drawn inside

    QColor solBlack(7, 54, 66);
    QColor solWhite(238, 232, 213);
    QColor solBrWhite(253, 246, 227);
    QColor solBlue(38, 139, 210);
    QColor solGreen(133, 153, 0);
    QColor solRed(220, 50, 47);

    QPen pen(solBlue, 3);
    QBrush background(solBlack);

    if (highlight_)
    {
        pen.setColor(solRed);
    }

    if (localHighlight_)
    {
        pen.setWidth(5);
        background.setColor(solBrWhite);
    }

    painter->setPen(pen);
    painter->setRenderHint(QPainter::RenderHint::HighQualityAntialiasing);
    painter->fillRect(rect, background);
    painter->drawRect(rect);
    painter->drawText(rect, Qt::AlignCenter, text_);
}

void ReteNodeItem::addEdge(ReteEdgeItem* edge)
{
    edges_.push_back(edge);
}


std::vector<ReteEdgeItem*> ReteNodeItem::edges() const
{
    return edges_;
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
