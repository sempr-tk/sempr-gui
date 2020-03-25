#ifndef SEMPR_GUI_HPP_
#define SEMPR_GUI_HPP_

#include <QtCore>
#include <QWidget>

#include "ECModel.hpp"
#include "AbstractInterface.hpp"

#include "../ui/ui_main.h"

namespace sempr { namespace gui {

/**
    The main GUI-widget. Contains different widgets for different views and
    editors. Handles the connection between the ECModel and SEMPR.
*/
class SemprGui : public QWidget {
    /// the connection to sempr
    AbstractInterface::Ptr semprInterface_;

    /// the local data model to be used by the different views
    ECModel dataModel_;

    // the main ui form
    Ui_Form form_;
public:
    SemprGui(AbstractInterface::Ptr interface);
    ~SemprGui();
};


}}

#endif /* include guard: SEMPR_GUI_HPP_ */
