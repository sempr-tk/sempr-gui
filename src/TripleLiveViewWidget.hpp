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
*/
class TripleLiveViewWidget : public QWidget {
    Q_OBJECT

    Ui::TripleLiveViewWidget* form_;

    // stack columns & filter duplicates, for the completer
    StackedColumnsProxyModel stackModel_;
    UniqueFilterProxyModel uniqueModel_;
    QCompleter completer_;

    // model with all the triples, plus a filter that takes all columns into
    // account
    QStandardItemModel allTriplesModel_;
    AnyColumnFilterProxyModel filterModel_;

    std::map<rete::Triple, int> tripleToIndex_;

protected slots:
    void onRequestMenu(const QPoint& point);

public:
    TripleLiveViewWidget(QWidget* parent = nullptr);
    ~TripleLiveViewWidget();

    void tripleUpdate(sempr::Triple, AbstractInterface::Notification);

signals:
    void requestExplanation(const QString& sub, const QString& pred, const QString& obj);
};

}}

#endif /* include guard: SEMPR_GUI_TRIPLELIVEVIEWWIDGET_HPP_ */
