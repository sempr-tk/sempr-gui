#ifndef SEMPR_GUI_EXPLANATIONEDGEITEM_HPP_
#define SEMPR_GUI_EXPLANATIONEDGEITEM_HPP_

#include <QGraphicsItem>
#include <QPainter>
#include "ExplanationNodeItem.hpp"

namespace sempr { namespace gui {

/**
    A QGraphicsItem to draw an edge between to two ExplanationNodes
*/
class ExplanationEdgeItem : public QGraphicsItem {
    ExplanationNodeItem *fromNode_, *toNode_;
    QPointF fromPoint_, toPoint_;
public:
    ExplanationEdgeItem(ExplanationNodeItem* from, ExplanationNodeItem* to);

    void adjust();
    ExplanationNodeItem* from() const;
    ExplanationNodeItem* to() const;

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
};


}}

#endif /* include guard: SEMPR_GUI_EXPLANATIONEDGEITEM_HPP_ */
