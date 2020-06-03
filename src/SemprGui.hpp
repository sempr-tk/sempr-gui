#ifndef SEMPR_GUI_HPP_
#define SEMPR_GUI_HPP_

#include <QtCore>
#include <QWidget>
#include <QTabWidget>
#include <QMenu>

#include "ECModel.hpp"
#include "AbstractInterface.hpp"
#include "UsefulWidget.hpp"

//#include "../ui/ui_main.h"

class Ui_Form; // forward declaration

namespace sempr { namespace gui {

/**
    The main GUI-widget. Contains different widgets for different views and
    editors.
*/
class SemprGui : public QWidget {

    /// the local data model to be used by the different views
    ECModel dataModel_;

    // the main ui form
    Ui_Form* form_;

    AbstractInterface::Ptr sempr_;
private slots:
    /**
        Adds/removes the widget tab to/from the tab widget.
    */
    void updateTabStatus(UsefulWidget* w, bool visible);

    /**
        Adds a entry to the history list that shows the latest updates from the
        sempr core. The second argument is prepended to the history entry.
    */
    void logUpdate(const sempr::gui::ECData&,
                   const QString&);

    /**
        Adds an entry to the error list
    */
    void logError(const QString&);


    /**
        Changes the layout of the extra-utility-widgets
    */
    void changeWidgetLayout(const QString& layout);
    // helper, moves the content to a visible tabwidget
    void emptyUtilTabWidget(QTabWidget*);

    /**
        Opens a context menu for the selected entity-component entry in the
        treeview
    */
    void onECCustomContextMenu(const QPoint& point);

    /**
        Handles the request (from another widget) to explain a triple
    */
    void onExplainRequest(const QString& s, const QString& p, const QString& o);
public:
    SemprGui(AbstractInterface::Ptr interface);
    ~SemprGui();
};


}}

#endif /* include guard: SEMPR_GUI_HPP_ */
