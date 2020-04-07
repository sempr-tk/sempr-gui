#include "ColoredBranchTreeView.hpp"
#include <QPainter>

namespace sempr { namespace gui {

ColoredBranchTreeView::ColoredBranchTreeView(QWidget* parent)
    : QTreeView(parent)
{
}

void ColoredBranchTreeView::drawBranches(
        QPainter* painter,
        const QRect& rect,
        const QModelIndex& index) const
{
    if (index.isValid())
    {
        auto bg = index.data(Qt::BackgroundRole);
        if (bg.canConvert<QColor>())
        {
            painter->fillRect(rect, bg.value<QColor>());
        }
    }

    QTreeView::drawBranches(painter, rect, index);
}

void ColoredBranchTreeView::dataChanged(
        const QModelIndex& tl, const QModelIndex& br,
        const QVector<int>& roles)
{
    /*
        Without this, the branches are not updated, but only the contents of
        the items. The update works by iterating the changed rows, getting
        their visual rectangle and repainting it. But befor repainting, it is
        expanded by twice the indentation -- one for the root, one for the
        actual item.
        This is very specific to my model with a depth of only one
        (parent+child), but the only alternative I see is to repaint the whole
        viewport with this->viewport()->repaint();
    */
    for (int i = tl.row(); i <= br.row(); i++)
    {
        auto index = tl.sibling(i, 0);
        auto rect = this->visualRect(index);
        rect.setLeft(rect.left() - 2*this->indentation());
        this->viewport()->repaint(rect);
    }
    QTreeView::dataChanged(tl, br, roles);
}

}}
