#ifndef SEMPR_GUI_TRIPLECONTAINERWIDGET_HPP_
#define SEMPR_GUI_TRIPLECONTAINERWIDGET_HPP_

#include <QtCore>
#include <QtWidgets>

#include "ECModel.hpp"
#include "SingleComponentWidget.hpp"

#include "StackedColumnsProxyModel.hpp"
#include "UniqueFilterProxyModel.hpp"

namespace Ui {
    class TripleContainerWidget;
}

namespace sempr {
    class TripleContainer;

    namespace gui {

/**
    The TripleContainerWidget is a read-only widget for displaying any component
    that implements the TripleContainer interface. An additional text field lets
    you filter the triples.
*/
class TripleContainerWidget : public SingleComponentWidget<TripleContainer> {
    Q_OBJECT

    Ui::TripleContainerWidget* form_;

    // two filters for the completer: 3 columns -> 1 column + remove duplicates
    StackedColumnsProxyModel* stackedColumnsProxy_;
    UniqueFilterProxyModel* uniqueProxy_;
    QCompleter* completer_;

    // a filter for the tree widget
    QSortFilterProxyModel* listFilterProxy_;

    // updates the widget with the data from the given component
    bool updateComponentWidget(std::shared_ptr<TripleContainer> component, bool isMutable) override;

protected slots:
    // updates the filter for the triple list based on the current
    // value of the line edit
    void updateFilter(const QString& text);
public:
    TripleContainerWidget(QWidget* parent = nullptr);
    ~TripleContainerWidget();
};

}}

#endif /* include guard: SEMPR_GUI_TRIPLECONTAINERWIDGET_HPP_ */
