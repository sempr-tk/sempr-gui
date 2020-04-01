#include "TripleVectorWidget.hpp"

#include <sempr/component/TripleVector.hpp>


namespace sempr { namespace gui {

TripleVectorWidget::TripleVectorWidget(QWidget* parent)
    : UsefulWidget(parent)
{
    form_.setupUi(this);

    // add a completer to the line edit
    completer_ = new QCompleter(this);

    // make the completer work on all parts of the triples, i.e. every column.
    // And make the choices unique.
    stackedColumnsProxy_ = new StackedColumnsProxyModel(this);
    stackedColumnsProxy_->setSourceModel(form_.treeWidget->model());
    uniqueProxy_ = new UniqueFilterProxyModel(this);
    uniqueProxy_->setSourceModel(stackedColumnsProxy_);

    completer_->setModel(uniqueProxy_);
    completer_->setFilterMode(Qt::MatchFlag::MatchContains);
    form_.lineEdit->setCompleter(completer_);

    connect(form_.lineEdit, &QLineEdit::textChanged,
            this, &TripleVectorWidget::updateFilter);

    connect(form_.btnAdd, &QPushButton::clicked,
            this, &TripleVectorWidget::add);
    connect(form_.btnRemove, &QPushButton::clicked,
            this, &TripleVectorWidget::remove);
    connect(form_.btnSave, &QPushButton::clicked,
            this, &TripleVectorWidget::save);
}


void TripleVectorWidget::add()
{
    auto item = new QTreeWidgetItem();
    item->setFlags(item->flags() | Qt::ItemFlag::ItemIsEditable);
    form_.treeWidget->insertTopLevelItem(0, item);
}


void TripleVectorWidget::remove()
{
    auto item = form_.treeWidget->currentItem();
    if (item)
    {
        int index = form_.treeWidget->indexOfTopLevelItem(item);
        item = form_.treeWidget->takeTopLevelItem(index);
        delete item;
    }
}

void TripleVectorWidget::save()
{
    bool okay = false;

    if (currentIndex_.isValid())
    {
        auto variant = model_->data(currentIndex_, Qt::UserRole);
        if (variant.canConvert<ModelEntry>())
        {
            auto entry = variant.value<ModelEntry>();
            auto vector = std::dynamic_pointer_cast<TripleVector>(entry.component_);
            if (vector)
            {
                // clear old data, construct completely new.
                vector->clear();

                // iterate the tree widget to fill it with the new values
                int numItems = form_.treeWidget->topLevelItemCount();
                for (int i = 0; i < numItems; i++)
                {
                    auto item = form_.treeWidget->topLevelItem(i);
                    sempr::Triple t;
                    t.setField(sempr::Triple::Field::SUBJECT, item->text(0).toStdString());
                    t.setField(sempr::Triple::Field::PREDICATE, item->text(1).toStdString());
                    t.setField(sempr::Triple::Field::OBJECT, item->text(2).toStdString());

                    vector->addTriple(t);
                }

                // trigger re-compuation of the json serialization
                okay = entry.setComponent(entry.component_);

                // tell the model to save this.
                model_->updateComponent(entry);
            }
        }
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


void TripleVectorWidget::updateWidget()
{
    bool useful = false;
    bool canEdit = false;

    if (currentIndex_.isValid())
    {
        auto variant = model_->data(currentIndex_, Qt::UserRole);
        if (variant.canConvert<ModelEntry>())
        {
            auto entry = variant.value<ModelEntry>();
            auto vector = std::dynamic_pointer_cast<TripleVector>(entry.component_);
            if (vector)
            {
                useful = true;
                canEdit = entry.mutable_;

                // first get rid of old data
                form_.lineEdit->clear();
                form_.treeWidget->clear();

                // then: fill with data
                for (auto triple : *vector)
                {
                    auto item = new QTreeWidgetItem();
                    item->setText(0, QString::fromStdString(triple.getField(Triple::Field::SUBJECT)));
                    item->setText(1, QString::fromStdString(triple.getField(Triple::Field::PREDICATE)));
                    item->setText(2, QString::fromStdString(triple.getField(Triple::Field::OBJECT)));

                    Qt::ItemFlags flags = item->flags();
                    if (canEdit)
                    {
                        flags = flags | Qt::ItemFlag::ItemIsEditable;
                    }
                    else
                    {
                        flags = flags & (~Qt::ItemFlag::ItemIsEditable);
                    }
                    item->setFlags(flags);

                    form_.treeWidget->addTopLevelItem(item);
                }
            }
        }
    }

    this->setUseful(useful);
}

}}
