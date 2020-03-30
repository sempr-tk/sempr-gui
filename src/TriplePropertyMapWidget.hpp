#ifndef SEMPR_GUI_TRIPLEPROPERTYMAPWIDGET_HPP_
#define SEMPR_GUI_TRIPLEPROPERTYMAPWIDGET_HPP_

#include <QtCore>
#include <QtWidgets>

#include "../ui/ui_triplepropertymap.h"
#include "ECModel.hpp"
#include "UsefulWidget.hpp"

namespace sempr { namespace gui {

class TriplePropertyMapWidget : public UsefulWidget {
    Q_OBJECT

    Ui_TriplePropertyMapWidget form_;

    /// Uses the currentIndex_ to access the data from the model and display it
    void updateWidget() override;

private slots:
    // saves the content of the table widget and submits it to the model -> to the core
    void save();

    // removes the currently selected entry from the TableWidget
    void remove();

    // adds a new entry to the TableWidget
    void add();

public:
    TriplePropertyMapWidget(QWidget* parent = nullptr);
};

}}

#endif /* include guard: SEMPR_GUI_TRIPLEPROPERTYMAPWIDGET_HPP_ */
