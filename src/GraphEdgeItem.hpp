#ifndef SEMPR_GUI_GRAPHEDGEITEM_HPP_
#define SEMPR_GUI_GRAPHEDGEITEM_HPP_

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include "GraphNodeItem.hpp"

namespace sempr { namespace gui {

/**
    A QGraphicsItem to draw an edge between two GraphNodeItems.
*/
class GraphEdgeItem : public QGraphicsItem {
    GraphNodeItem *fromNode_, *toNode_;
    QPointF fromPoint_, toPoint_;

    bool localHighlight_, globalHighlight_;
public:
    GraphEdgeItem(GraphNodeItem* from, GraphNodeItem* to);

    /// re-compute start and end points
    void adjust();

    void setLocalHighlighted(bool);
    void setGlobalHighlighted(bool);

    GraphNodeItem* from() const;
    GraphNodeItem* to() const;

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
};

}}

#endif /* include guard: SEMPR_GUI_GRAPHEDGEITEM_HPP_ */
