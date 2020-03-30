#ifndef SEMPR_GUI_HPP_
#define SEMPR_GUI_HPP_

#include <QtCore>
#include <QWidget>

#include "ECModel.hpp"
#include "AbstractInterface.hpp"
#include "UsefulWidget.hpp"

#include "../ui/ui_main.h"

namespace sempr { namespace gui {

/**
    The main GUI-widget. Contains different widgets for different views and
    editors.
*/
class SemprGui : public QWidget {

    /// the local data model to be used by the different views
    ECModel dataModel_;

    // the main ui form
    Ui_Form form_;

private slots:
    /**
        Adds/removes the widget tab to/from the tab widget.
    */
    void updateTabStatus(UsefulWidget* w, bool visible);

    /**
        Adds a entry to the history list that shows the latest updates from the
        sempr core. The second argument is prepended to the history entry.
    */
    void logUpdate(const sempr::gui::ModelEntry&,
                   const QString&);

public:
    SemprGui(AbstractInterface::Ptr interface);
    ~SemprGui();
};


}}

#endif /* include guard: SEMPR_GUI_HPP_ */
