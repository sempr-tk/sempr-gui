#include "TriplePropertyMapWidget.hpp"
#include <sempr/component/TriplePropertyMap.hpp>

namespace sempr { namespace gui {

TriplePropertyMapWidget::TriplePropertyMapWidget(QWidget* parent)
    : UsefulWidget(parent)
{
    form_.setupUi(this);

    connect(form_.btnRemove, &QPushButton::clicked,
            this, &TriplePropertyMapWidget::remove);
    connect(form_.btnAdd, &QPushButton::clicked,
            this, &TriplePropertyMapWidget::add);
    connect(form_.btnSave, &QPushButton::clicked,
            this, &TriplePropertyMapWidget::save);
}

void TriplePropertyMapWidget::remove()
{
    auto rows = form_.tableWidget->selectionModel()->selectedRows();
    if (rows.size() > 0)
    {
        // only remove the first selected as I guess that the oder indices get
        // invalidated by this
        form_.tableWidget->removeRow(rows[0].row());
    }
    else
    {
        QMessageBox msg( QMessageBox::Icon::Warning, "No row selected",
                         "Please select an entire row to delete.",
                         QMessageBox::Button::Ok );
        msg.exec();
    }
}


void TriplePropertyMapWidget::save()
{
    bool okay = false;

    if (currentIndex_.isValid())
    {
        auto variant = model_->data(currentIndex_, Qt::UserRole);
        if (variant.canConvert<ModelEntry>())
        {
            auto entry = variant.value<ModelEntry>();
            auto map = std::dynamic_pointer_cast<TriplePropertyMap>(entry.component_);

            // okay, got the component... clear its data!
            map->map_.clear();

            // then iterate over the table widget and add new data
            size_t numRows = form_.tableWidget->rowCount();
            for (size_t i = 0; i < numRows; i++)
            {
                QString key = form_.tableWidget->item(i, 0)->text();
                QString value = form_.tableWidget->item(i, 1)->text();
                auto cellWidget = form_.tableWidget->cellWidget(i, 2);
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
            okay = entry.setComponent(entry.component_);

            // tell the model to update this component
            model_->updateComponent(entry);
        }
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
    form_.tableWidget->setRowCount(form_.tableWidget->rowCount()+1);

    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItem("int", TriplePropertyMapEntry::INT);
    typeCombo->addItem("float", TriplePropertyMapEntry::FLOAT);
    typeCombo->addItem("string", TriplePropertyMapEntry::STRING);
    typeCombo->addItem("resource", TriplePropertyMapEntry::RESOURCE);

    typeCombo->setCurrentIndex(0);

    form_.tableWidget->setCellWidget(form_.tableWidget->rowCount()-1, 2, typeCombo);
}


void TriplePropertyMapWidget::updateWidget()
{
    // check if the current index is valid, if we can get a ModelEntry and if
    // it contains a pointer to a TriplePropertyMap:
    bool canDisplay = false;
    bool canEdit = false;
    if (currentIndex_.isValid())
    {
        auto variant = model_->data(currentIndex_, Qt::UserRole);
        if (variant.canConvert<ModelEntry>())
        {
            ModelEntry entry = variant.value<ModelEntry>();

            auto map = std::dynamic_pointer_cast<TriplePropertyMap>(entry.component_);
            if (map)
            {
                canDisplay = true;
                canEdit = entry.mutable_;

                // first, make sure that the current view is empty
                form_.tableWidget->clear();
                size_t numProps = map->map_.size();
                form_.tableWidget->setColumnCount(3);
                form_.tableWidget->setRowCount(numProps);

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

                    form_.tableWidget->setItem(currentRow, 0, keyItem);
                    form_.tableWidget->setItem(currentRow, 1, valueItem);
                    form_.tableWidget->setCellWidget(currentRow, 2, typeCombo);
                    currentRow++;
                } // end for every property in the map
            } // end if map is a TriplePropertyMap
        } // end if variant holds a ModelEntry
    } // end if current index is valid

    this->setEnabled(canEdit);
    this->setUseful(canDisplay);
}

}}
