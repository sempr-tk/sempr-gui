#include "ComponentAdderWidget.hpp"
#include "../ui/ui_componentadderwidget.h"

#include "CustomDataRoles.hpp"
#include "ModelEntry.hpp"

#include <sempr/component/AffineTransform.hpp>
#include <sempr/component/TripleDocument.hpp>
#include <sempr/component/TriplePropertyMap.hpp>
#include <sempr/component/TripleVector.hpp>
#include <sempr/component/TextComponent.hpp>
#include <cereal/archives/json.hpp>
#include <sstream>

namespace sempr { namespace gui {

ComponentAdderWidget::ComponentAdderWidget(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::ComponentAdderWidget)
{
    ui_->setupUi(this);

    registerComponentType<AffineTransform>("AffineTransform");
    registerComponentType<TripleDocument>("TripleDocument");
    registerComponentType<TripleVector>("TripleVector");
    registerComponentType<TriplePropertyMap>("TriplePropertyMap");
    registerComponentType<TextComponent>("TextComponent");

    connect(ui_->btnCreate, &QPushButton::clicked,
            this, &ComponentAdderWidget::createComponent);

    connect(ui_->rbtnAuto, &QRadioButton::toggled,
            ui_->inputEntityId, &QLineEdit::setDisabled);

    rebuildComboBox();
}

void ComponentAdderWidget::createComponent()
{
    QString name = ui_->comboEntityType->currentText();
    std::string entityId("");
    if (ui_->rbtnManual->isChecked())
    {
        entityId = ui_->inputEntityId->text().toStdString();
    }

    auto component = creatorFunctions_[name]();

    model_->addComponent(component, entityId, "");
}

void ComponentAdderWidget::setModel(ECModel* model)
{
    model_ = model;
}

void ComponentAdderWidget::setSelectionModel(QItemSelectionModel* model)
{
    selectionModel_ = model;
    connect(model, &QItemSelectionModel::currentRowChanged,
            this, &ComponentAdderWidget::onCurrentRowChanged);
}


void ComponentAdderWidget::registerComponentType(const QString& name, creatorFunction_t creator)
{
    creatorFunctions_[name] = creator;
    rebuildComboBox();
}

void ComponentAdderWidget::onCurrentRowChanged(
        const QModelIndex& current,
        const QModelIndex& /*previous*/)
{
    if (current.isValid() && model_)
    {
        auto var = model_->data(current, Role::EntityIdRole);
        ui_->inputEntityId->setText(var.toString());
    }
}

void ComponentAdderWidget::rebuildComboBox()
{
    ui_->comboEntityType->clear();
    for (auto it = creatorFunctions_.begin(); it != creatorFunctions_.end(); it++)
    {
        ui_->comboEntityType->addItem(it.key());
    }
    ui_->comboEntityType->setCurrentIndex(0);
}

ComponentAdderWidget::~ComponentAdderWidget()
{
    delete ui_;
}

}}
