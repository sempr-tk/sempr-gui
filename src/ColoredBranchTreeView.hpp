#ifndef SEMPR_GUI_COLOREDBRANCHTREEVIEW_HPP_
#define SEMPR_GUI_COLOREDBRANCHTREEVIEW_HPP_

#include <QTreeView>

namespace sempr { namespace gui {


/**
    Extends QTreeView to fill the branches colour based on the BackgroundRole.
*/
class ColoredBranchTreeView : public QTreeView {
public:
    ColoredBranchTreeView(QWidget* parent = nullptr);

    void drawBranches(QPainter*, const QRect&, const QModelIndex&) const override;

    /**
        Also re-draw the branches when the background role changes!
    */
    void dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&) override;
};


}}

#endif /* include guard: SEMPR_GUI_COLOREDBRANCHTREEVIEW_HPP_ */
