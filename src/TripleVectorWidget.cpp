#include "TripleVectorWidget.hpp"
#include "../ui/ui_triplevector.h"
#include "CustomDataRoles.hpp"

namespace sempr { namespace gui {

TripleVectorWidget::TripleVectorWidget(QWidget* parent)
    : SingleComponentWidget(parent), form_(new Ui::TripleVectorWidget)
{
    form_->setupUi(this);

    // add a completer to the line edit
    completer_ = new QCompleter(this);

    // make the completer work on all parts of the triples, i.e. every column.
    // And make the choices unique.
    stackedColumnsProxy_ = new StackedColumnsProxyModel(this);
    stackedColumnsProxy_->setSourceModel(form_->treeWidget->model());
    uniqueProxy_ = new UniqueFilterProxyModel(this);
    uniqueProxy_->setSourceModel(stackedColumnsProxy_);

    completer_->setModel(uniqueProxy_);
    completer_->setFilterMode(Qt::MatchFlag::MatchContains);
    form_->lineEdit->setCompleter(completer_);

    connect(form_->lineEdit, &QLineEdit::textChanged,
            this, &TripleVectorWidget::updateFilter);

    connect(form_->btnAdd, &QPushButton::clicked,
            this, &TripleVectorWidget::add);
    connect(form_->btnRemove, &QPushButton::clicked,
            this, &TripleVectorWidget::remove);


    // save to the model whenever a TreeWidgetItem is edited by the user
    connect(form_->treeWidget, &QTreeWidget::itemChanged,
            this, &TripleVectorWidget::save);
}

TripleVectorWidget::~TripleVectorWidget()
{
    delete form_;
}


void TripleVectorWidget::add()
{
    auto item = new QTreeWidgetItem();
    item->setFlags(item->flags() | Qt::ItemFlag::ItemIsEditable);
    form_->treeWidget->insertTopLevelItem(0, item);

    // update the model
    save();
}


void TripleVectorWidget::remove()
{
    auto item = form_->treeWidget->currentItem();
    if (item)
    {
        int index = form_->treeWidget->indexOfTopLevelItem(item);
        item = form_->treeWidget->takeTopLevelItem(index);
        delete item;
    }

    // update the model
    save();
}

void TripleVectorWidget::save()
{
    bool okay = false;
    auto vector = currentPtr();
    if (vector)
    {
        // clear old data, construct completely new.
        vector->clear();

        // iterate the tree widget to fill it with the new values
        int numItems = form_->treeWidget->topLevelItemCount();
        for (int i = 0; i < numItems; i++)
        {
            auto item = form_->treeWidget->topLevelItem(i);
            sempr::Triple t;
            t.setField(sempr::Triple::Field::SUBJECT, item->text(0).toStdString());
            t.setField(sempr::Triple::Field::PREDICATE, item->text(1).toStdString());
            t.setField(sempr::Triple::Field::OBJECT, item->text(2).toStdString());

            vector->addTriple(t);
        }

        // update the model
        okay = model_->setData(
                currentIndex_,
                QVariant::fromValue(std::static_pointer_cast<Component>(vector)),
                Role::ComponentPtrRole);
    }

    if (!okay)
    {
        QMessageBox msg(QMessageBox::Icon::Critical, "Error",
                "Something went wrong while saving the component.",
                QMessageBox::Button::Ok);
        msg.exec();
    }
}


void TripleVectorWidget::updateFilter(const QString& text)
{
    int numItems = form_->treeWidget->topLevelItemCount();
    for (int i = 0; i < numItems; i++)
    {
        auto item = form_->treeWidget->topLevelItem(i);
        bool show =  (item->text(0).contains(text, Qt::CaseInsensitive) ||
                item->text(1).contains(text, Qt::CaseInsensitive) ||
                item->text(2).contains(text, Qt::CaseInsensitive));
        item->setHidden(!show);
    }
}


bool TripleVectorWidget::updateComponentWidget(TripleVector::Ptr vector, bool isMutable)
{
    if (vector)
    {
        // first get rid of old data
        form_->lineEdit->clear();
        form_->treeWidget->clear();

        // then: fill with data
        for (auto triple : *vector)
        {
            auto item = new QTreeWidgetItem();
            item->setText(0, QString::fromStdString(triple.getField(Triple::Field::SUBJECT)));
            item->setText(1, QString::fromStdString(triple.getField(Triple::Field::PREDICATE)));
            item->setText(2, QString::fromStdString(triple.getField(Triple::Field::OBJECT)));

            Qt::ItemFlags flags = item->flags();
            if (isMutable)
            {
                flags = flags | Qt::ItemFlag::ItemIsEditable;
            }
            else
            {
                flags = flags & (~Qt::ItemFlag::ItemIsEditable);
            }
            item->setFlags(flags);

            form_->treeWidget->addTopLevelItem(item);
        }

        return true;
    }

    return false;
}

}}
