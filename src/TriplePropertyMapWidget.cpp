#include "TriplePropertyMapWidget.hpp"
#include "../ui/ui_triplepropertymap.h"
#include "CustomDataRoles.hpp"

namespace sempr { namespace gui {

TriplePropertyMapWidget::TriplePropertyMapWidget(QWidget* parent)
    : SingleComponentWidget(parent), form_(new Ui::TriplePropertyMapWidget)
{
    form_->setupUi(this);

    connect(form_->btnRemove, &QPushButton::clicked,
            this, &TriplePropertyMapWidget::remove);
    connect(form_->btnAdd, &QPushButton::clicked,
            this, &TriplePropertyMapWidget::add);

    // update after every change of an item
    connect(form_->tableWidget, &QTableWidget::cellChanged,
            this, &TriplePropertyMapWidget::save);
}

TriplePropertyMapWidget::~TriplePropertyMapWidget()
{
    delete form_;
}

void TriplePropertyMapWidget::remove()
{

    auto rows = form_->tableWidget->selectionModel()->selectedRows();
    if (rows.size() > 0)
    {
        form_->tableWidget->blockSignals(true);
        // only remove the first selected as I guess that the oder indices get
        // invalidated by this
        form_->tableWidget->removeRow(rows[0].row());
        form_->tableWidget->blockSignals(false);
    }
    else
    {
        QMessageBox msg( QMessageBox::Icon::Warning, "No row selected",
                         "Please select an entire row to delete.",
                         QMessageBox::Button::Ok );
        msg.exec();
    }

    // update the model
    save();
}


void TriplePropertyMapWidget::save()
{
    bool okay = false;

    auto map = currentPtr();
    if (map)
    {
        // okay, got the component... clear its data!
        map->map_.clear();

        // then iterate over the table widget and add new data
        size_t numRows = form_->tableWidget->rowCount();
        for (size_t i = 0; i < numRows; i++)
        {
            QString key = form_->tableWidget->item(i, 0)->text();
            QString value = form_->tableWidget->item(i, 1)->text();
            auto cellWidget = form_->tableWidget->cellWidget(i, 2);
            auto comboWidget = dynamic_cast<QComboBox*>(cellWidget);
            if (comboWidget)
            {
                auto data = comboWidget->currentData();
                // shouldn't even need to check this...
                if (data.canConvert<int>()) // TriplePropertyMapEntry::Type is an enum, should be good.
                {
                    auto type = data.value<int>();

                    // now, create the new entry.
                    if (type == TriplePropertyMapEntry::INT)
                    {
                        map->map_[key.toStdString()] = value.toInt();
                    }
                    else if (type == TriplePropertyMapEntry::FLOAT)
                    {
                        map->map_[key.toStdString()] = value.toFloat();
                    }
                    else if (type == TriplePropertyMapEntry::RESOURCE)
                    {
                        map->map_[key.toStdString()] = { value.toStdString(), true };
                    }
                    else /*if (type == TriplePropertyMapEntry::STRING)*/
                    {
                        map->map_[key.toStdString()] = value.toStdString();
                    }
                }
                else
                {
                    // :(
                    throw std::exception();
                }
            }
            else
            {
                // wtf?
                throw std::exception();
            }
        }

        // trigger re-computation of the json representation
        okay = model_->setData(
                currentIndex_,
                QVariant::fromValue(std::static_pointer_cast<Component>(map)),
                Role::ComponentPtrRole);

        // Only set the data in the model.
        // When it is actually sent to the sempr core is not our business!
    }

    if (!okay)
    {
        QMessageBox msg(QMessageBox::Icon::Critical, "Error",
                "Something went wrong while trying to save the component.",
                QMessageBox::Button::Ok);
        msg.exec();
    }
}


void TriplePropertyMapWidget::add()
{
    form_->tableWidget->blockSignals(true);

    form_->tableWidget->setRowCount(form_->tableWidget->rowCount()+1);

    QTableWidgetItem* keyItem = new QTableWidgetItem();
    QTableWidgetItem* valueItem = new QTableWidgetItem();
    form_->tableWidget->setItem(form_->tableWidget->rowCount()-1, 0, keyItem);
    form_->tableWidget->setItem(form_->tableWidget->rowCount()-1, 1, valueItem);

    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItem("int", TriplePropertyMapEntry::INT);
    typeCombo->addItem("float", TriplePropertyMapEntry::FLOAT);
    typeCombo->addItem("string", TriplePropertyMapEntry::STRING);
    typeCombo->addItem("resource", TriplePropertyMapEntry::RESOURCE);

    typeCombo->setCurrentIndex(0);

    // call save whenever the combo box selection changes
    connect(typeCombo, &QComboBox::currentTextChanged,
            this, &TriplePropertyMapWidget::save);

    form_->tableWidget->setCellWidget(form_->tableWidget->rowCount()-1, 2, typeCombo);

    form_->tableWidget->blockSignals(false);

    // update the model
    save();
}


bool TriplePropertyMapWidget::updateComponentWidget(
        TriplePropertyMap::Ptr map, bool isMutable)
{
    if (!map)
    {
        this->setEnabled(false);
        return false;
    }

    // first, make sure that the current view is empty
    form_->tableWidget->clear();
    size_t numProps = map->map_.size();
    form_->tableWidget->setColumnCount(3);
    form_->tableWidget->setRowCount(numProps);

    // block signals -- must not "save" to the model yet, that would result
    // in endless recursion.
    form_->tableWidget->blockSignals(true);

    // then, add the entries: 3 for every property in the map.
    // 1: key
    // 2: value
    // 3: type
    int currentRow = 0;
    for (auto entry : map->map_)
    {
        QTableWidgetItem* keyItem = new QTableWidgetItem();
        keyItem->setText(QString::fromStdString(entry.first));

        QTableWidgetItem* valueItem = new QTableWidgetItem();
        int intVal;
        float fltVal;
        switch(entry.second.type()) {
            case TriplePropertyMapEntry::INT:
                intVal = entry.second;
                valueItem->setText(QString::number(intVal));
                break;
            case TriplePropertyMapEntry::FLOAT:
                fltVal = entry.second;
                valueItem->setText(QString::number(fltVal));
                break;
            case TriplePropertyMapEntry::STRING:
            case TriplePropertyMapEntry::RESOURCE:
                {
                    std::string strVal = entry.second;
                    valueItem->setText(QString::fromStdString(strVal));
                    break;
                }
            case TriplePropertyMapEntry::INVALID:
                // ???
                break;
        }

        QComboBox* typeCombo = new QComboBox();
        typeCombo->addItem("int", TriplePropertyMapEntry::INT);
        typeCombo->addItem("float", TriplePropertyMapEntry::FLOAT);
        typeCombo->addItem("string", TriplePropertyMapEntry::STRING);
        typeCombo->addItem("resource", TriplePropertyMapEntry::RESOURCE);
        switch (entry.second.type()) {
            case TriplePropertyMapEntry::INT:
                typeCombo->setCurrentText("int");
                break;
            case TriplePropertyMapEntry::FLOAT:
                typeCombo->setCurrentText("float");
                break;
            case TriplePropertyMapEntry::STRING:
                typeCombo->setCurrentText("string");
                break;
            case TriplePropertyMapEntry::RESOURCE:
                typeCombo->setCurrentText("resource");
                break;
            case TriplePropertyMapEntry::INVALID:
                // ???
                break;
        }

        // call save whenever the combo box selection changes
        connect(typeCombo, &QComboBox::currentTextChanged,
                this, &TriplePropertyMapWidget::save);

        form_->tableWidget->setItem(currentRow, 0, keyItem);
        form_->tableWidget->setItem(currentRow, 1, valueItem);
        form_->tableWidget->setCellWidget(currentRow, 2, typeCombo);
        currentRow++;
    } // end for every property in the map

    form_->tableWidget->blockSignals(false);

    this->setEnabled(isMutable);
    return true;
}

}}
