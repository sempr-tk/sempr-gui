#ifndef SEMPR_GUI_TRIPLELIVEVIEWWIDGET_HPP_
#define SEMPR_GUI_TRIPLELIVEVIEWWIDGET_HPP_

#include <QtWidgets>
#include <QStandardItemModel>

#include "AbstractInterface.hpp"
#include "AnyColumnFilterProxyModel.hpp"
#include "StackedColumnsProxyModel.hpp"
#include "UniqueFilterProxyModel.hpp"

#include <QCompleter>
#include <map>

namespace Ui {
    class TripleLiveViewWidget;
}

namespace sempr { namespace gui {


/**
    The TripleLiveViewWidget provides access to a list of all triples which
    can be filtered, and is updated live while the data in the sempr core
    changes. Very similar to the entity-component view; though, this is a
    strictly read-only thing.
    Aside from a plain list of triples, this widget includes a form for
    SPARQL SELECT queries.
*/
class TripleLiveViewWidget : public QWidget {
    Ui::TripleLiveViewWidget* form_;
    AbstractInterface::Ptr sempr_;

    // stack columns & filter duplicates, for the completer
    StackedColumnsProxyModel stackModel_;
    UniqueFilterProxyModel uniqueModel_;
    QCompleter completer_;

    // model with all the triples, plus a filter that takes all columns into
    // account
    QStandardItemModel allTriplesModel_;
    AnyColumnFilterProxyModel filterModel_;

    std::map<rete::Triple, int> tripleToIndex_;


    void tripleUpdate(sempr::Triple, AbstractInterface::Notification);
public:
    TripleLiveViewWidget(QWidget* parent = nullptr);
    ~TripleLiveViewWidget();

    // stores the connection object and sets its triple callback to update
    // this widget
    void setConnection(AbstractInterface::Ptr);
};

}}

#endif /* include guard: SEMPR_GUI_TRIPLELIVEVIEWWIDGET_HPP_ */
