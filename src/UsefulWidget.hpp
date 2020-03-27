#ifndef SEMPR_GUI_USEFULWIDGET_HPP_
#define SEMPR_GUI_USEFULWIDGET_HPP_

#include <QWidget>


namespace sempr { namespace gui {

/**
    The UsefulWidget is just a small helper class: I want to have a set of
    widgets that decide themselves if they are useful in the current situation
    (based on selected data), and are removed from a tab widget if not.
    Therefore this class provides a setUseful method which emits a corresponding
    signal.
*/
class UsefulWidget : public QWidget {
    Q_OBJECT
public:
    /**
        Create a new UsefulWidget.
    */
    UsefulWidget(QWidget* parent = nullptr);

    void setUseful(bool);
signals:
    /**
        Signals if the widget can currently display meaningfull data.
    */
    void isUseful(UsefulWidget*, bool);
};


}}


#endif /* include guard: SEMPR_GUI_USEFULWIDGET_HPP_ */
