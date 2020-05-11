#ifndef SEMPR_GUI_ZOOMGRAPHICSVIEW_HPP_
#define SEMPR_GUI_ZOOMGRAPHICSVIEW_HPP_

#include <QGraphicsView>

namespace sempr { namespace gui {

/**
    A QGraphicsView that supports zooming in/out with the mouse wheel,
    and dragging the scene while holding the middl mouse button.
*/
class ZoomGraphicsView : public QGraphicsView {
    bool moving_;
    QPoint lastPoint_;
public:
    ZoomGraphicsView(QWidget* parent = nullptr);

    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};


}}


#endif /* include guard: SEMPR_GUI_ZOOMGRAPHICSVIEW_HPP_ */
