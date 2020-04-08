#include "RawComponentWidget.hpp"
#include "CustomDataRoles.hpp"

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

    bool ok = model_->setData(currentIndex_, form_.rawComponentEdit->toPlainText(),
                              Role::ComponentJsonRole);
    // TODO: what to do when the json is not ok?
    // just ignore for now, might be that the component type is just not known
    // ... could add an info label, change the save buttons colour, whatever.
    // Somehow signal that things... worked, or did not.

    // Note: We don't tell the model to send the data to the sempr core anymore.
    // Instead, we keep the change in a kind of "staging area", and will send it
    // when explicitely called for. This allows for a clearer differentiation
    // between the models data, and the data in the sempr core, gives us a way
    // to reset things, etc.
}


void RawComponentWidget::updateWidget()
{
    if (currentIndex_.isValid())
    {
        auto varJson = model_->data(currentIndex_, Role::ComponentJsonRole);
        auto varMut = model_->data(currentIndex_, Role::ComponentMutableRole);
        if (varJson.canConvert<QString>() && varMut.canConvert<bool>())
        {
            bool mut = varMut.value<bool>();
            QString json = varJson.value<QString>();

            form_.rawComponentEdit->setPlainText(json);

            // enable text entry and save button only if the component is
            // mutable!
            //
            //this->setEnabled(entry.mutable_);
            // disabling the edit is a bit too much: Cannot read it properly
            // anymore due to the grey background, and no selection for
            // copy & paste is possible anymore. Instead, disable the button,
            // and set the edit to read only.
            this->setEnabled(true);
            form_.btnSave->setEnabled(mut);
            form_.rawComponentEdit->setReadOnly(!mut);

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
        this->setEnabled(false);

        // signal that this widget isn't useful right now
        setUseful(false);
    }
}


}}
