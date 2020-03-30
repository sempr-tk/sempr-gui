#ifndef SEMPR_GUI_USEFULWIDGET_HPP_
#define SEMPR_GUI_USEFULWIDGET_HPP_

#include <QWidget>

#include "ECModel.hpp"


namespace sempr { namespace gui {

/**
    The UsefulWidget is just a small helper class: I want to have a set of
    widgets that decide themselves if they are useful in the current situation
    (based on selected data), and are removed from a tab widget if not.
    Therefore this class provides a setUseful method which emits a corresponding
    signal.
    It also stores a reference to the ECModel and the item selection model and
    provides slots that call a virtual updateWidget method if the data or
    selection has changed.
*/
class UsefulWidget : public QWidget {
    Q_OBJECT

protected:
    /// stores the index of the currently selected item
    QPersistentModelIndex currentIndex_;

    /// access to the model, in order to view/edit entries
    ECModel* model_;
    QItemSelectionModel* selectionModel_;

    /**
        Called after either the currently selected data or the selection itself
        changed.
    */
    virtual void updateWidget() = 0;

protected slots:
    /**
        Checks if the currentIndex is inside of the region that changed as
        defined by topleft and bottomright, and calls updateWidget().
    */
    virtual void onDataChanged(const QModelIndex& topleft,
                               const QModelIndex& bottomright,
                               const QVector<int>& roles);

    /**
        Sets currentIndex_ to current and calls updateWidget().
    */
    virtual void onCurrentRowChanged(const QModelIndex& current,
                                     const QModelIndex& previous);

public:
    /**
        Create a new UsefulWidget.
    */
    UsefulWidget(QWidget* parent = nullptr);

    void setModel(ECModel* model);
    void setSelectionModel(QItemSelectionModel* model);

    void setUseful(bool);

signals:
    /**
        Signals if the widget can currently display meaningfull data.
    */
    void isUseful(UsefulWidget*, bool);
};


}}


#endif /* include guard: SEMPR_GUI_USEFULWIDGET_HPP_ */
