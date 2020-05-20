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
}


void TripleLiveViewWidget::setConnection(AbstractInterface::Ptr con)
{
    if (sempr_) sempr_->clearTripleUpdateCallback();

    allTriplesModel_.clear();
    allTriplesModel_.setHorizontalHeaderLabels({"subject", "predicate", "object"});

    sempr_ = con;

    if (sempr_)
    {
        sempr_->setTripleUpdateCallback(
                std::bind(
                    &TripleLiveViewWidget::tripleUpdate,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2
                )
        );

        // initialize with data, the callback will only be called for changes
        auto triples = sempr_->listTriples();
        for (auto& t : triples)
        {
            tripleUpdate(t, AbstractInterface::Notification::ADDED);
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
