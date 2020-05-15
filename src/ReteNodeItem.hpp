#ifndef SEMPR_GUI_RETENODEITEM_HPP_
#define SEMPR_GUI_RETENODEITEM_HPP_

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <vector>

namespace sempr { namespace gui {
    class ReteEdgeItem;

/**
    A QGraphicsItem to draw a rete node.
    Ellipse + Text.
*/
class ReteNodeItem : public QGraphicsItem {
    QString text_;

    // whether to highlight this node or not
    bool highlight_;

    // highlight used internally, for this node and its edges on hover
    bool localHighlight_;

    std::vector<ReteEdgeItem*> edges_;
public:
    ReteNodeItem(const QString& text);
    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

    // adds the edge item to the nodes list.
    // will call ReteEdgeItem::adjust when the nodes position changed
    void addEdge(ReteEdgeItem* edge);
    std::vector<ReteEdgeItem*> edges() const;

    void setHighlighted(bool);
    bool isHighlighted() const;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    // highlighth node locally on hover
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
};

}}

#endif /* include guard: SEMPR_GUI_RETENODEITEM_HPP_ */
