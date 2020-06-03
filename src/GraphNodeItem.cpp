#include "GraphNodeItem.hpp"
#include "GraphEdgeItem.hpp"

#include <QFontDatabase>
#include <QFontMetrics>
#include <QApplication>
#include <QGraphicsView>
#include <cmath>

namespace sempr { namespace gui {

GraphNodeItem::GraphNodeItem(const QString& text, Shape s)
    : text_(text), shape_(s), highlight_(false), localHighlight_(false)
{
    setCacheMode(DeviceCoordinateCache);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsMovable);
    setZValue(0);
    setAcceptHoverEvents(true);
}


QRectF GraphNodeItem::drawingRect() const
{
    auto rect = textRect();

    if (shape_ == Shape::Ellipse)
    {
        // ellipse -> scale up to fit the text!
        // sqrt(2)? Seems to easy, but it's what the equations tell us...
        // The idea is to look at the difference between the length of the
        // half-diagonal (d) and the radius (r) of the ellipse where both
        // intersect. Then just scale up the whole ellipse proportionally to
        // make the intersection appear at the corner of the rectangle.
        // Well, using pythagoras, the simple ellipse equation
        // (x/a)^2+(y/b)^2 = 1 and the equality of tangens ~> y/x = (h/2)/(w/2)
        // it turns out that d/r = .... sqrt(2). Independent of h,w. Maybe I
        // made an error somewhere in there...? But it seems to work. I guess
        // sometimes it's just that simple. :)
        rect.setWidth(rect.width() * M_SQRT2);
        rect.setHeight(rect.height() * M_SQRT2);
        rect.moveCenter({0, 0});

        // also, add a constant margin
        rect.adjust(-5, -5, 5, 5);
    }
    else /* if (shape_ == Shape::Rectangle) */
    {
        rect.adjust(-10, -10, 10, 10);
    }

    return rect;
}

QRectF GraphNodeItem::boundingRect() const
{
    return drawingRect().adjusted(-5, -5, 5, 5);
}

QRectF GraphNodeItem::textRect() const
{
    QFontMetricsF fm(font());
    auto rect = fm.boundingRect(QRect(), 0, text_);
    rect.moveCenter({0, 0});
    return rect;
}

QFont GraphNodeItem::font() const
{
    // select a font.
    // fallback: application wide default font
    QFont font = QApplication::font();
    // but if there is at least one view this is shown in, take the font of
    // that
    if (scene() && !scene()->views().isEmpty() && scene()->views()[0])
    {
        auto info = scene()->views()[0]->fontInfo();
        font = QFont(info.family(), info.pointSize(), info.weight(), info.italic());
    }

    return font;
}

void GraphNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    auto rect = drawingRect();
    auto txtRect = textRect();

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

    if (shape_ == Shape::Rectangle)
    {
        painter->fillRect(rect, background);
        painter->drawRect(rect);
    }
    else if (shape_ == Shape::Ellipse)
    {
        painter->setBrush(background);
        painter->drawEllipse(rect);
    }

    QFont pFont = font();
    painter->setFont(pFont);

    painter->drawText(txtRect, Qt::AlignCenter, text_);
}

void GraphNodeItem::addEdge(GraphEdgeItem* edge)
{
    edges_.push_back(edge);
}


std::vector<GraphEdgeItem*> GraphNodeItem::edges() const
{
    return edges_;
}

QVariant GraphNodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
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


double GraphNodeItem::ellipseRadiusAtVectorIntersection(double a, double b, double dx, double dy)
{
    if (abs(dx * dy) < 10e-10)
    {
        if (abs(dy) < 10e-7) return a;
        if (abs(dx) < 10e-7) return b;
    }

    double tana = dy/dx;
    double xsq = (a*a*b*b)/(b*b+a*a*tana*tana);
    double ysq = tana*tana * xsq;

    return sqrt(xsq+ysq);
}

void GraphNodeItem::setHighlighted(bool on)
{
    highlight_ = on;
    update();
}

bool GraphNodeItem::isHighlighted() const
{
    return highlight_;
}



void GraphNodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* e)
{
    localHighlight_ = true;
    update();

    for (auto e : edges_)
    {
        e->setLocalHighlighted(true);
    }

    QGraphicsItem::hoverEnterEvent(e);
}

void GraphNodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* e)
{
    localHighlight_ = false;
    update();

    for (auto e : edges_)
    {
        e->setLocalHighlighted(false);
    }

    QGraphicsItem::hoverLeaveEvent(e);
}



void GraphNodeItem::calculateForces()
{
    if (!scene() || scene()->mouseGrabberItem() == this)
    {
        dynamicNewPos_ = pos();
        return;
    }

    if (text_ == "Root")
    {
        dynamicNewPos_ = pos();
        return;
    }


    auto thisRect = boundingRect();

    // first part: push away from other nodes
    qreal xvel = 0;
    qreal yvel = 0;
    const QList<QGraphicsItem*> items = scene()->items();


    QMap<GraphNodeItem*, double> distances;

    for (auto item : items)
    {
        if (item == this) continue;

        GraphNodeItem* node = dynamic_cast<GraphNodeItem*>(item);
        if (!node) continue;


        QPointF vec = mapToItem(node, 0, 0);
        qreal dx = vec.x();
        qreal dy = vec.y();
        double l = (dx*dx + dy*dy);


        // subtract the length of the connecting lines inside the objects.
        // -> dont push centers away, but borders!
        // approximated by ellipses.
        //
        auto thisRect = boundingRect();
        auto otherRect = node->boundingRect();

        double lengthInsideThis = ellipseRadiusAtVectorIntersection(
                                    thisRect.width()/2., thisRect.height()/2.,
                                    dx, dy);
        double lengthInsideOther = ellipseRadiusAtVectorIntersection(
                                    otherRect.width()/2., otherRect.height()/2.,
                                    -dx, -dy);

        double lengthInside = lengthInsideThis + lengthInsideOther;
        // if lengthInside >= length: contact.
        // actual distance: length - lengthInside.
        double lengthDiff = l - lengthInside*lengthInside;
        if (lengthDiff < 0)
        {
            l = 1./(abs(lengthDiff)+10.);
        }
        else
            l = lengthDiff;

        distances[node] = l;

        //if (l > 0)// && l < 200*200)
        {
            double dxvel = (dx*30.)/(abs(l)+10);
            double dyvel = (dy*30.)/(abs(l)+10);

            xvel += dxvel*abs(dxvel);
            yvel += dyvel*abs(dyvel);
//            xvel += (dxvel*dxvel);
//            yvel += (dyvel*dyvel);
        }
    }


    // second part: pull together by edges
    double weight = (edges_.size() + 1) * 50;
    for (auto edge : edges_)
    {
        GraphNodeItem* other;
        if (edge->from() == this) other = edge->to();
        else                      other = edge->from();

        QPointF vec = mapToItem(other, 0, 0);

        xvel -= vec.x() / weight;
        yvel -= vec.y() / weight;
    }

    if (qAbs(xvel) < 0.1) xvel = 0;
    if (qAbs(yvel) < 0.1) yvel = 0;
    if (qAbs(xvel) > 5.0) xvel = 5. * (xvel < 0 ? -1. : 1.);
    if (qAbs(yvel) > 5.0) yvel = 5. * (yvel < 0 ? -1. : 1.);

    dynamicNewPos_ = pos() + QPointF(xvel, yvel);

    auto sceneRect = scene()->sceneRect();
    dynamicNewPos_.setX(
            qMin(qMax(dynamicNewPos_.x(), sceneRect.left() + thisRect.width()/2.),
                                          sceneRect.right() - thisRect.width()/2.));

    dynamicNewPos_.setY(
            qMin(qMax(dynamicNewPos_.y(), sceneRect.top() + thisRect.height()/2.),
                                          sceneRect.bottom() - thisRect.height()/2.));

}

bool GraphNodeItem::advancePosition()
{
    if (dynamicNewPos_ == pos()) return false;
    setPos(dynamicNewPos_);
    return true;
}


}}
