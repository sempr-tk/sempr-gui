#include "TripleLiveViewWidget.hpp"
#include "../ui/ui_tripleliveviewwidget.h"

namespace sempr { namespace gui {

TripleLiveViewWidget::TripleLiveViewWidget(QWidget* parent)
    : QWidget(parent), form_(new Ui::TripleLiveViewWidget)
{
    form_->setupUi(this);

    allTriplesModel_.setColumnCount(3);

    // stack & make unique, for the completer
    stackModel_.setSourceModel(&allTriplesModel_);
    uniqueModel_.setSourceModel(&stackModel_);

    completer_.setModel(&uniqueModel_);
    completer_.setFilterMode(Qt::MatchContains);

    form_->tripleListFilterEdit->setCompleter(&completer_);

    // set the filter model for displaying matching triples
    filterModel_.setSourceModel(&allTriplesModel_);

    form_->tripleList->setModel(&filterModel_);

    connect(form_->tripleListFilterEdit, &QLineEdit::textChanged,
            &filterModel_,
            QOverload<const QString&>::of(&AnyColumnFilterProxyModel::setFilterFixedString));

    form_->tripleList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(form_->tripleList, &QTreeView::customContextMenuRequested,
            this, &TripleLiveViewWidget::onRequestMenu);
}


void TripleLiveViewWidget::onRequestMenu(const QPoint& point)
{
    auto index = form_->tripleList->indexAt(point);
    if (index.isValid())
    {
        QMenu menu;
        auto explainAction = menu.addAction("explain");
        auto requestedAction = menu.exec(form_->tripleList->viewport()->mapToGlobal(point));
        if (requestedAction == explainAction)
        {
            auto s = index.sibling(index.row(), 0).data().toString();
            auto p = index.sibling(index.row(), 1).data().toString();
            auto o = index.sibling(index.row(), 2).data().toString();

            emit requestExplanation(s, p, o);
        }
    }
}


void TripleLiveViewWidget::tripleUpdate(
        sempr::Triple triple,
        AbstractInterface::Notification flag)
{
    // create a rete::Triple from the sempr::Triple, because the rete::Triple
    // is already suited to be stored in a map or set.
    rete::Triple rt(
        triple.getField(sempr::Triple::Field::SUBJECT),
        triple.getField(sempr::Triple::Field::PREDICATE),
        triple.getField(sempr::Triple::Field::OBJECT)
    );

    if (flag == AbstractInterface::Notification::ADDED)
    {
        // remember the new row index for the triple
        tripleToIndex_[rt] = allTriplesModel_.rowCount();

        auto s = new QStandardItem(QString::fromStdString(rt.subject));
        auto p = new QStandardItem(QString::fromStdString(rt.predicate));
        auto o = new QStandardItem(QString::fromStdString(rt.object));

        // append to the model
        allTriplesModel_.appendRow({s, p, o});
    }
    else if (flag == AbstractInterface::Notification::REMOVED)
    {
        // remove the triple from the model
        int index = tripleToIndex_[rt];
        allTriplesModel_.removeRow(index);

        // remove it from the triple-to-index map
        tripleToIndex_.erase(rt);
        // update the triple-to-index-map, all following triples have a lower
        // row number now
        for (auto& entry : tripleToIndex_)
        {
            if (entry.second > index) tripleToIndex_[entry.first] = (entry.second-1);
        }
    }
    else
    {
        // must never happen. triples cannot be updated
        throw std::exception();
    }
}


TripleLiveViewWidget::~TripleLiveViewWidget()
{
    delete form_;
}


}}
