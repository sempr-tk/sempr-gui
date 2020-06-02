#include "ExplanationNodeItem.hpp"
#include "ExplanationEdgeItem.hpp"

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <cmath>

namespace sempr { namespace gui {

ExplanationNodeItem::ExplanationNodeItem(const QString& text)
    : text_(text)
{
//    setAcceptHoverEvents(true);
    setCacheMode(DeviceCoordinateCache);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsMovable);
    setZValue(0);
}


QRectF ExplanationNodeItem::drawingRect() const
{
    auto rect = textRect();
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

    return rect;
}

QRectF ExplanationNodeItem::boundingRect() const
{
    return drawingRect().adjusted(-5, -5, 5, 5);
}

QRectF ExplanationNodeItem::textRect() const
{
    QFontMetricsF fm(font());
    auto rect = fm.boundingRect(QRect(), 0, text_);
    rect.moveCenter({0, 0});
    return rect;
}

QFont ExplanationNodeItem::font() const
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


void ExplanationNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
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

    painter->setPen(pen);
    painter->setRenderHint(QPainter::RenderHint::HighQualityAntialiasing);

    painter->setBrush(background);
    painter->drawEllipse(rect);

    QFont pFont = font();
    painter->setFont(pFont);

    painter->drawText(txtRect, Qt::AlignCenter, text_);
}

void ExplanationNodeItem::addEdge(ExplanationEdgeItem* edge)
{
    edges_.push_back(edge);
}


std::vector<ExplanationEdgeItem*> ExplanationNodeItem::edges() const
{
    return edges_;
}

QVariant ExplanationNodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
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
