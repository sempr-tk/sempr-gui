#include "RawComponentWidget.hpp"

namespace sempr { namespace gui {

RawComponentWidget::RawComponentWidget(QWidget* parent)
    : UsefulWidget(parent)
{
    form_.setupUi(this);

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
    bool ok = currentEntry.setJSON(form_.rawComponentEdit->toPlainText().toStdString());

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


}}
