#ifndef SEMPR_GUI_EXPLANATIONNODEITEM_HPP_
#define SEMPR_GUI_EXPLANATIONNODEITEM_HPP_

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include "ExplanationNode.hpp"

namespace sempr { namespace gui {
    class ExplanationEdgeItem;


/**
    A QGraphicsItem to draw a node of an ExplanationGraph
*/
class ExplanationNodeItem : public QGraphicsItem {
    QString text_;
    std::vector<ExplanationEdgeItem*> edges_;

    static double ellipseRadiusAtVectorIntersection(double a, double b, double dx, double dy);

    QFont font() const;
public:
    ExplanationNodeItem(const QString& text);

    QRectF drawingRect() const; // region to draw in, slightly smaller than boundingRect
    QRectF textRect() const; // size the text will occupy
    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void addEdge(ExplanationEdgeItem* edge);
    std::vector<ExplanationEdgeItem*> edges() const;

//    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;


};


}}


#endif /* include guard: SEMPR_GUI_EXPLANATIONNODEITEM_HPP_ */
