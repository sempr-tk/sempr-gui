#include "ReteNodeItem.hpp"
#include <QFontMetrics>
#include <QApplication>
#include "ReteEdgeItem.hpp"

#include <iostream>
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>

namespace sempr { namespace gui {

ReteNodeItem::ReteNodeItem(const QString& text)
    : text_(text), highlight_(false), localHighlight_(false)
{
    setAcceptHoverEvents(true);
    setCacheMode(DeviceCoordinateCache);
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


void ReteNodeItem::calculateForces()
{
    if (!scene() || scene()->mouseGrabberItem() == this)
    {
        dynamicNewPos_ = pos();
        return;
    }

    auto thisRect = boundingRect();

    // first part: push away from other nodes
    qreal xvel = 0;
    qreal yvel = 0;
    const QList<QGraphicsItem*> items = scene()->items();


    QMap<ReteNodeItem*, double> distances;

    for (auto item : items)
    {
        ReteNodeItem* node = qgraphicsitem_cast<ReteNodeItem*>(item);
        if (!node) continue;

        QPointF vec = mapToItem(node, 0, 0);
        qreal dx = vec.x();
        qreal dy = vec.y();
        double l = (dx*dx + dy*dy);
        double alpha = atan2(dy, dx);

        // adjust the length by taking the shape of the nodes in account!
        // at least approximately.
        // -- length inside the rectangle of this item
        /*
        double distTopDown = fabs(thisRect.width()/(2.*cos(alpha)));
        double distLeftRight = fabs(thisRect.height()/(2.*sin(alpha)));
        double distInsideThis = std::min(distTopDown, distLeftRight);
        distInsideThis = distInsideThis * distInsideThis;

        // -- length inside the rectangle of the other item
        alpha += M_PI;
        distTopDown = fabs(otherRect.width()/(2.*cos(alpha)));
        distLeftRight = fabs(otherRect.height()/(2.*sin(alpha)));
        double distInsideOther = std::min(distTopDown, distLeftRight);
        distInsideOther = distInsideOther * distInsideOther;
        */
        auto otherRect = node->boundingRect();
        //double distInsideThis = std::max(thisRect.width(), thisRect.height())/2.;
        //double distInsideOther = std::max(otherRect.width(), otherRect.height())/2.;
        //double distInsideThis = (thisRect.width() + thisRect.height())/2.;
        //double distInsideOther = (otherRect.width() + otherRect.height())/2.;
        double distInsideThis = thisRect.height()/2.;
        double distInsideOther = otherRect.height()/2.;

        distInsideThis = distInsideThis * distInsideThis;
        distInsideOther = distInsideOther * distInsideOther;

        // subtract length of edge inside nodes from the distance of the
        // midpoints
        //l = l - distInsideThis - distInsideOther;
        distances[node] = l;

        if (l < 0)
        {
//            qDebug() << this->text_ << "contact with" << node->text_;
        }

        if (l > 0 && l < 100*100)
        {
            xvel += (dx * 50.)/l;
            yvel += (dy * 50.)/l;
        }
    }


    // second part: pull together by edges
    double weight = (edges_.size() + 1) * 50;
    for (auto edge : edges_)
    {
        ReteNodeItem* other;
        if (edge->from() == this) other = edge->to();
        else                      other = edge->from();

        QPointF vec = mapToItem(other, 0, 0);

        if (distances[other] > 20)
        {
            xvel -= vec.x() / weight;
            yvel -= vec.y() / weight;
        }
        else
        {
            xvel += vec.x() / weight;
            yvel += vec.y() / weight;
        }
    }

    if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
        xvel = yvel = 0;

    dynamicNewPos_ = pos() + QPointF(xvel, yvel);

    auto sceneRect = scene()->sceneRect();
    dynamicNewPos_.setX(
            qMin(qMax(dynamicNewPos_.x(), sceneRect.left() + thisRect.width()/2.),
                                          sceneRect.right() - thisRect.width()/2.));

    dynamicNewPos_.setY(
            qMin(qMax(dynamicNewPos_.y(), sceneRect.top() + thisRect.height()/2.),
                                          sceneRect.bottom() - thisRect.height()/2.));

}

bool ReteNodeItem::advancePosition()
{
    if (dynamicNewPos_ == pos()) return false;
    setPos(dynamicNewPos_);
    return true;
}


}}
