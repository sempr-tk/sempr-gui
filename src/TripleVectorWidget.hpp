#ifndef SEMPR_GUI_TRIPLEVECTORWIDGET_HPP_
#define SEMPR_GUI_TRIPLEVECTORWIDGET_HPP_

#include <sempr/component/TripleVector.hpp>
#include "SingleComponentWidget.hpp"

#include "../ui/ui_triplevector.h"

#include "StackedColumnsProxyModel.hpp"
#include "UniqueFilterProxyModel.hpp"

#include <QtWidgets>

namespace sempr { namespace gui {

/**
    This widget is similar to the TripleContainerWidget, but provides editing
    of the entries.
*/
class TripleVectorWidget : public SingleComponentWidget<TripleVector> {
    Q_OBJECT

    Ui_TripleVectorWidget form_;

    // for the completion in the search bar
    StackedColumnsProxyModel* stackedColumnsProxy_;
    UniqueFilterProxyModel* uniqueProxy_;
    QCompleter* completer_;

    bool updateComponentWidget(std::shared_ptr<TripleVector> vector, bool isMutable) override;

protected slots:
    /// hides entries in the tree widget that do not match the search
    void updateFilter(const QString& text);

    /// rebuilds the triplevector from the current set of tree items
    void save();

    /// appends a tree item
    void add();

    /// removes the currently selected item
    void remove();

public:
    TripleVectorWidget(QWidget* parent = nullptr);
};


}}

#endif /* include guard: SEMPR_GUI_TRIPLEVECTORWIDGET_HPP_ */
