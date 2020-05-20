#include "DragDropTabBar.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QApplication>
#include <QMimeData>

#include <QDrag>

namespace sempr { namespace gui {

DragDropTabBar::DragDropTabBar(QWidget* parent)
    : QTabBar(parent)
{
    setAcceptDrops(true);
}

void DragDropTabBar::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) dragStart_ = event->pos();
    QTabBar::mousePressEvent(event);
}


void DragDropTabBar::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if ((dragStart_ - event->pos()).manhattanLength() > QApplication::startDragDistance())
        {
            auto drag = new QDrag(this);

            auto rect = this->tabRect(this->currentIndex());
            QPixmap pix = this->grab(rect);

            drag->setPixmap(pix);
            QMimeData* mime = new QMimeData();
            drag->setMimeData(mime);

            drag->exec();
        }
    }

    QTabBar::mouseMoveEvent(event);
}


void DragDropTabBar::dragEnterEvent(QDragEnterEvent* event)
{
    auto obj = event->source();
    auto tabbar = dynamic_cast<DragDropTabBar*>(obj);

    if (tabbar)
    {
        event->acceptProposedAction();
    }
}

void DragDropTabBar::dropEvent(QDropEvent* event)
{
    auto obj = event->source();
    auto tabbar = dynamic_cast<DragDropTabBar*>(obj);

    if(tabbar)
    {
        int hoveredIndex = this->tabAt(event->pos());
        if (hoveredIndex < 0) return;

        int otherIndex = tabbar->currentIndex();

        auto otherWidget = tabbar->myWidget_->widget(otherIndex);
        auto otherLabel = tabbar->myWidget_->tabText(otherIndex);

        tabbar->myWidget_->removeTab(otherIndex);

        int insertIndex = hoveredIndex+1;
        if (tabbar == this && hoveredIndex >= otherIndex) insertIndex = hoveredIndex;

        myWidget_->insertTab(insertIndex, otherWidget, otherLabel);

        myWidget_->setCurrentWidget(otherWidget);

        event->acceptProposedAction();
    }
}


// ------------------------------
// DragDropTabWidget
// ------------------------------

DragDropTabWidget::DragDropTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    auto tabbar = new DragDropTabBar(this);
    tabbar->myWidget_ = this;

    setTabBar(tabbar);
    setAcceptDrops(true);
}


void DragDropTabWidget::dragEnterEvent(QDragEnterEvent* event)
{
    auto obj = event->source();
    auto tabbar = dynamic_cast<DragDropTabBar*>(obj);

    // the widget only accepts it if it has no tabs.
    // else, everything should be handled by the tabbar.
    if (tabbar && this->currentIndex() == -1)
    {
        event->acceptProposedAction();
    }
}

void DragDropTabWidget::dropEvent(QDropEvent* event)
{
    auto obj = event->source();
    auto tabbar = dynamic_cast<DragDropTabBar*>(obj);

    if (tabbar && this->currentIndex() == -1)
    {
        int otherIndex = tabbar->currentIndex();
        auto otherWidget = tabbar->myWidget_->widget(otherIndex);
        auto otherLabel = tabbar->myWidget_->tabText(otherIndex);

        tabbar->myWidget_->removeTab(otherIndex);
        this->insertTab(0, otherWidget, otherLabel);

        event->acceptProposedAction();
    }
}



}}
