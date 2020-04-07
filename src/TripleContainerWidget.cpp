#include "TripleContainerWidget.hpp"

#include <sempr/component/TripleContainer.hpp>

namespace sempr { namespace gui {

TripleContainerWidget::TripleContainerWidget(QWidget* parent)
    : SingleComponentWidget(parent)
{
    form_.setupUi(this);

    // add a completer for the line edit
    completer_ = new QCompleter(this);
    // make it work on all components of the triples, not only subjects, by
    // stacking the columns on top of each other
    stackedColumnsProxy_ = new StackedColumnsProxyModel(this);
    stackedColumnsProxy_->setSourceModel(form_.treeWidget->model());

    // remove duplicate values
    uniqueProxy_ = new UniqueFilterProxyModel(this);
    uniqueProxy_->setSourceModel(stackedColumnsProxy_);

    completer_->setModel(uniqueProxy_);
    // find anything containing the search string
    completer_->setFilterMode(Qt::MatchFlag::MatchContains);
    // set the completer
    form_.lineEdit->setCompleter(completer_);

    // make the line edit apply a filter to the view of the tree widget
    connect(form_.lineEdit, &QLineEdit::textChanged,
            this, &TripleContainerWidget::updateFilter);
}


void TripleContainerWidget::updateFilter(const QString& text)
{
    // sadly, setModel is a private member of QTreeWidget, and I'm not going to
    // implement a whole model-based approach here. So just iterate over all
    // items and show/hide them, manually.

    int numItems = form_.treeWidget->topLevelItemCount();
    for (int i = 0; i < numItems; i++)
    {
        auto item = form_.treeWidget->topLevelItem(i);
        bool show =  (item->text(0).contains(text, Qt::CaseInsensitive) ||
                      item->text(1).contains(text, Qt::CaseInsensitive) ||
                      item->text(2).contains(text, Qt::CaseInsensitive));
        item->setHidden(!show);
    }
}

bool TripleContainerWidget::updateComponentWidget(
        TripleContainer::Ptr container, bool /*isMutable*/)
{
    // Note: TripleContainerWidget cannot modify anything, as TripleContainer
    // is a read-only interface. -> isMutable is ignored.
    if (container)
    {
        // first: clear.
        form_.lineEdit->clear();
        form_.treeWidget->clear();

        // then: fill with data
        for (auto triple : *container)
        {
            auto item = new QTreeWidgetItem();
            item->setText(0, QString::fromStdString(triple.getField(Triple::Field::SUBJECT)));
            item->setText(1, QString::fromStdString(triple.getField(Triple::Field::PREDICATE)));
            item->setText(2, QString::fromStdString(triple.getField(Triple::Field::OBJECT)));

            form_.treeWidget->addTopLevelItem(item);
        }

        return true;
    }

    return false;
}

}}
