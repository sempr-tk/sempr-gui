#ifndef SEMPR_GUI_TRIPLECONTAINERWIDGET_HPP_
#define SEMPR_GUI_TRIPLECONTAINERWIDGET_HPP_

#include <QtCore>
#include <QtWidgets>

#include "../ui/ui_triplecontainer.h"
#include "ECModel.hpp"
#include "UsefulWidget.hpp"
#include "StackedColumnsProxyModel.hpp"
#include "UniqueFilterProxyModel.hpp"

namespace sempr { namespace gui {

/**
    The TripleContainerWidget is a read-only widget for displaying any component
    that implements the TripleContainer interface. An additional text field lets
    you filter the triples.
*/
class TripleContainerWidget : public UsefulWidget {
    Q_OBJECT

    Ui_TripleContainerWidget form_;

    // two filters for the completer: 3 columns -> 1 column + remove duplicates
    StackedColumnsProxyModel* stackedColumnsProxy_;
    UniqueFilterProxyModel* uniqueProxy_;
    QCompleter* completer_;

    // a filter for the tree widget
    QSortFilterProxyModel* listFilterProxy_;

    /// Uses the currentIndex_ to access the data from the model and display it
    void updateWidget() override;

protected slots:
    // updates the filter for the triple list based on the current
    // value of the line edit
    void updateFilter(const QString& text);
public:
    TripleContainerWidget(QWidget* parent = nullptr);
};

}}

#endif /* include guard: SEMPR_GUI_TRIPLECONTAINERWIDGET_HPP_ */
