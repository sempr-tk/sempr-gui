#ifndef SEMPR_GUI_GRAPHNODEITEM_HPP_
#define SEMPR_GUI_GRAPHNODEITEM_HPP_

#include <QGraphicsItem>
#include <vector>

namespace sempr { namespace gui {
    class GraphEdgeItem;

/**
    A QGraphicsItem that represents the node of a graph and contains
    some text.
*/
class GraphNodeItem : public QGraphicsItem {
public:
    enum Shape { Ellipse, Rectangle };
private:
    QPointF dynamicNewPos_; // for dynamic force positioning
protected:
    QString text_;
    Shape shape_;

    std::vector<GraphEdgeItem*> edges_;

    // whether to highlight this node or not
    bool highlight_;

    // highlight used internally, for this node and its edges on hover
    bool localHighlight_;


    /**
        Selecting a font is not that easy. Since we need it quite often, there
        is a function for it.
    */
    QFont font() const;

    /**
        The default shape for GraphNodeItems is an ellipse, in which case this
        helper comes in handy for distance calculations.
        Given an axis aligned ellipse with parameters a and b, this function
        returns the distance between the center of the ellipse (0,0) and the
        intersection point of the vector (dx, dy) with the border of the
        ellipse.
    */
    static double ellipseRadiusAtVectorIntersection(double a, double b, double dx, double dy);

    /**
        Analogon to ellipseRadiusAtVectorIntersection, just for rectangles.
    */
    static double rectangleRadiusAtVectorIntersection(double width, double height, double dx, double dy);

public:
    GraphNodeItem(const QString& text, Shape s = Shape::Ellipse);

    QRectF boundingRect() const override; // region to update
    QRectF drawingRect() const; // region to draw in, slightly smaller than
                                // boundingRect to accomodate for line thickness
    QRectF textRect() const; // size that the text will occupy

    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

    void addEdge(GraphEdgeItem* edge);
    std::vector<GraphEdgeItem*> edges() const;


    /**
        Returns the distance between the items (0,0) and its border in the
        specified direction.
    */
    double innerRadiusInDirection(double dx, double dy) const;

    /**
        Adjusts the connected edges whenever the node is moved
    */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    void setHighlighted(bool);
    bool isHighlighted() const;

    // highlight node locally on hover
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    // test dynamic positioning through forces between nodes and edges,
    // see the qt elastic nodes example
    void calculateForces();
    bool advancePosition();
};

}}


#endif /* include guard: SEMPR_GUI_GRAPHNODEITEM_HPP_ */
