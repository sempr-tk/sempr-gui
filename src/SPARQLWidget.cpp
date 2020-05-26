
#include "../ui/ui_sparqlwidget.h"

#include "SPARQLWidget.hpp"
#include "SPARQLItem.hpp"

namespace sempr { namespace gui {

SPARQLWidget::SPARQLWidget(QWidget* parent)
    : QWidget(parent), form_(new Ui::SPARQLWidget)
{
    form_->setupUi(this);
    form_->queryList->setModel(&queries_);

    connect(form_->btnQuery, &QPushButton::clicked, this, &SPARQLWidget::submitClicked);
    connect(form_->queryList, &QAbstractItemView::clicked, this, &SPARQLWidget::queryItemClicked);
}


void SPARQLWidget::queryItemClicked(const QModelIndex& index)
{
    if (!form_->queryResultView->model()) form_->queryResultView->setModel(&queries_);
    form_->queryResultView->setRootIndex(index);
    form_->queryEdit->setPlainText(index.data(Qt::UserRole+1).toString());

    // set the header data to reflect the variables from the query
    auto item = dynamic_cast<SPARQLItem*>(queries_.itemFromIndex(index));
    if (item)
    {
        queries_.setColumnCount(item->variableCount());
        for (size_t i = 0; i < item->variableCount(); i++)
        {
            queries_.setHeaderData(i, Qt::Horizontal, item->variableName(i));
        }
    }
}


void SPARQLWidget::update(sempr::Triple triple, AbstractInterface::Notification action)
{
    if (action == AbstractInterface::Notification::ADDED)
    {
        soprano_.addTriple(
            triple.getField(sempr::Triple::Field::SUBJECT),
            triple.getField(sempr::Triple::Field::PREDICATE),
            triple.getField(sempr::Triple::Field::OBJECT)
        );
    }
    else if (action == AbstractInterface::Notification::REMOVED)
    {
        soprano_.removeTriple(
            triple.getField(sempr::Triple::Field::SUBJECT),
            triple.getField(sempr::Triple::Field::PREDICATE),
            triple.getField(sempr::Triple::Field::OBJECT)
        );
    }
    else
    {
        throw std::exception(); // should never happen, triples cannot be updated
    }
}


void SPARQLWidget::submitClicked()
{
    SPARQLQuery query;
    query.query = form_->queryEdit->toPlainText().toStdString();
    soprano_.answer(query);

    auto item = new SPARQLItem();
    item->update(query);
    queries_.appendRow(item);

    auto lastIndex = queries_.index(queries_.rowCount()-1, 0);
    form_->queryList->selectionModel()->setCurrentIndex(lastIndex, QItemSelectionModel::SelectionFlag::ClearAndSelect);
    queryItemClicked(lastIndex);
}


SPARQLWidget::~SPARQLWidget()
{
    delete form_;
}

}}
