#ifndef SEMPR_GUI_DRAGDROPTABBAR_HPP_
#define SEMPR_GUI_DRAGDROPTABBAR_HPP_

#include <QTabWidget>
#include <QTabBar>
#include <QDrag>

namespace sempr { namespace gui {
    class DragDropTabWidget;

/**
    A custom TabBar that allows dragging a tab from one TabWidget to another.
*/
class DragDropTabBar : public QTabBar {
    QPoint dragStart_;
    DragDropTabWidget* myWidget_;

    friend class DragDropTabWidget;
public:
    DragDropTabBar(QWidget* parent = nullptr);
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

    void dragEnterEvent(QDragEnterEvent*) override;
    void dropEvent(QDropEvent*) override;
};


/**
    I noticed that you cannot set the TabBar of a QTabWidget...
    Hence this small helper.
*/
class DragDropTabWidget : public QTabWidget {
public:
    DragDropTabWidget(QWidget* parent);

    void dragEnterEvent(QDragEnterEvent*) override;
    void dropEvent(QDropEvent*) override;
};


}}

#endif /* include guard: SEMPR_GUI_DRAGDROPTABBAR_HPP_ */
