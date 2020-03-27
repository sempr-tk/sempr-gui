#ifndef SEMPR_GUI_RAWCOMPONENTWIDGET_HPP_
#define SEMPR_GUI_RAWCOMPONENTWIDGET_HPP_

#include <QtCore>
#include <QtWidgets>

#include "../ui/ui_raw_component.h"
#include "ECModel.hpp"
#include "UsefulWidget.hpp"

namespace sempr { namespace gui {

class RawComponentWidget : public UsefulWidget {
    Q_OBJECT

    Ui_RawComponent form_;

    /// stores the index of the currently selected item
    QPersistentModelIndex currentIndex_;

    /// access to the model, in order to view/edit entries
    ECModel* model_;
    QItemSelectionModel* selectionModel_;

    /// Uses the currentIndex_ to access the data from the model and display it
    void updateWidget();

private slots:
    // saves the content of the edit field in the currently selected component
    void save();

public:
    RawComponentWidget(QWidget* parent = nullptr);

    void setModel(ECModel* model);
    void setSelectionModel(QItemSelectionModel* model);

public slots:
    /**
        Updates the widget if the currently selected item has been changed
    */
    void onDataChanged(const QModelIndex& topleft, const QModelIndex& bottomright, const QVector<int>& roles);

    /**
        Updates the widget when a different item was selected
    */
    void onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous);
};

}}

#endif /* include guard: SEMPR_GUI_RAWCOMPONENTWIDGET_HPP_ */
