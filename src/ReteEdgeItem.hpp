#ifndef SEMPR_GUI_RETEEDGEITEM_HPP_
#define SEMPR_GUI_RETEEDGEITEM_HPP_

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include "ReteNodeItem.hpp"

namespace sempr { namespace gui {

/**
    A QGraphicsItem to draw an edge between two ReteNodeItems.
*/
class ReteEdgeItem : public QGraphicsItem {
    ReteNodeItem *fromNode_, *toNode_;
    QPointF fromPoint_, toPoint_;

    bool localHighlight_, globalHighlight_;
public:
    ReteEdgeItem(ReteNodeItem* from, ReteNodeItem* to);

    /// re-compute start and end points
    void adjust();

    void setLocalHighlighted(bool);
    void setGlobalHighlighted(bool);

    ReteNodeItem* from() const;
    ReteNodeItem* to() const;

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
};

}}

#endif /* include guard: SEMPR_GUI_RETEEDGEITEM_HPP_ */
