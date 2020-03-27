#include "RawComponentWidget.hpp"

namespace sempr { namespace gui {

RawComponentWidget::RawComponentWidget(QWidget* parent)
    : UsefulWidget(parent), model_(nullptr), selectionModel_(nullptr)
{
    form_.setupUi(this);
    this->setWindowTitle("raw");

    connect(form_.btnSave, &QPushButton::clicked,
            this, &RawComponentWidget::save);
}


void RawComponentWidget::save()
{
    if (!currentIndex_.isValid()) return;
    if (!model_) return;

    // get the entry
    auto variant = model_->data(currentIndex_, Qt::UserRole);
    if (!variant.canConvert<ModelEntry>()) return;

    ModelEntry currentEntry = variant.value<ModelEntry>();
    // set the new json string
    auto ok = currentEntry.setJSON(form_.rawComponentEdit->toPlainText().toStdString());

    // TODO: what to do when the json is not ok?
    // just ignore for now, might be that the component type is just not known

    // send the update to the sempr core.
    // currently, this removes the current entry and adds a new one... :/
    model_->updateComponent(currentEntry);
}


void RawComponentWidget::updateWidget()
{
    if (currentIndex_.isValid())
    {
        auto variant = model_->data(currentIndex_, Qt::UserRole);
        if (variant.canConvert<ModelEntry>())
        {
            ModelEntry entry = variant.value<ModelEntry>();
            form_.rawComponentEdit->setPlainText(QString::fromStdString(entry.componentJSON_));

            // enable text entry and save button only if the component is
            // mutable!
            this->setEnabled(entry.mutable_);

            // signal this this widget is useful right now
            setUseful(true);
        }
        else
        {
            this->setEnabled(false);
            // couldn't get a model entry, so clear the edit
            form_.rawComponentEdit->setPlainText("");

            // signal that his widget isn't useful right now
            setUseful(false);
        }
    }
    else
    {
        // nothing selected.
        form_.rawComponentEdit->setPlainText("");
        form_.rawComponentEdit->setEnabled(false);
        form_.btnSave->setEnabled(false);

        // signal that this widget isn't useful right now
        setUseful(false);
    }
}


void RawComponentWidget::onDataChanged(
        const QModelIndex& topleft, const QModelIndex& bottomright,
        const QVector<int>&)
{
    if (!currentIndex_.isValid()) return;

    // to find the current item between topleft and bottomright, they must have
    // the same parent
    if (topleft.parent() == currentIndex_.parent())
    {
        if (topleft.row() <= currentIndex_.row() &&
            bottomright.row() >= currentIndex_.row())
        {
            // -> item behind currentIndex was updated!
            updateWidget();
        }
    }
}

void RawComponentWidget::onCurrentRowChanged(const QModelIndex& current, const QModelIndex&)
{
    currentIndex_ = QPersistentModelIndex(current);
    updateWidget();
}



void RawComponentWidget::setModel(ECModel* model)
{
    // disconnect from previous model
    if (model_)
    {
        disconnect(model_, &ECModel::dataChanged,
                   this, &RawComponentWidget::onDataChanged);
    }

    // set new model
    model_ = model;

    // connect to dataChanged signal
    if(model_)
    {
        connect(model_, &ECModel::dataChanged,
                this, &RawComponentWidget::onDataChanged);
    }
}


void RawComponentWidget::setSelectionModel(QItemSelectionModel* model)
{
    // disconnect from previous
    if (selectionModel_)
    {
        disconnect(selectionModel_, &QItemSelectionModel::currentRowChanged,
                   this, &RawComponentWidget::onCurrentRowChanged);
    }

    // set new model
    selectionModel_ = model;

    // connect to currentRowCanged signal
    if (selectionModel_)
    {
        connect(selectionModel_, &QItemSelectionModel::currentRowChanged,
                this, &RawComponentWidget::onCurrentRowChanged);
    }
}


}}
