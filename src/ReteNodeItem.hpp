#ifndef SEMPR_GUI_RETENODEITEM_HPP_
#define SEMPR_GUI_RETENODEITEM_HPP_

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <vector>

#include "ReteVisualSerialization.hpp"

namespace sempr { namespace gui {
    class ReteEdgeItem;

/**
    A QGraphicsItem to draw a rete node.
    Ellipse + Text.
*/
class ReteNodeItem : public QGraphicsItem {
    // memory, condition, or production node?
    Node::Type type_;

    QString text_;

    // whether to highlight this node or not
    bool highlight_;

    // highlight used internally, for this node and its edges on hover
    bool localHighlight_;

    std::vector<ReteEdgeItem*> edges_;

    QFont font() const;
public:
    ReteNodeItem(Node::Type type, const QString& text);

    QRectF boundingRect() const override; // region to update

    QRectF drawingRect() const; // region to draw in, slightly smaller than boundingRect
    QRectF textRect() const; // size the text will occupy

    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

    // adds the edge item to the nodes list.
    // will call ReteEdgeItem::adjust when the nodes position changed
    void addEdge(ReteEdgeItem* edge);
    std::vector<ReteEdgeItem*> edges() const;

    void setHighlighted(bool);
    bool isHighlighted() const;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    // highlight node locally on hover
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;


    // test dynamic positioning through forces between nodes and edges,
    // see the qt elastic nodes example
    void calculateForces();
    bool advancePosition();
private:
    QPointF dynamicNewPos_;
};

}}

#endif /* include guard: SEMPR_GUI_RETENODEITEM_HPP_ */
