#include "SemprGui.hpp"

namespace sempr { namespace gui {

SemprGui::SemprGui(AbstractInterface::Ptr interface)
    : dataModel_(interface)
{
    form_.setupUi(this);
    // both views use the same model
    form_.treeView->setModel(&dataModel_);
    auto selectionModel = form_.treeView->selectionModel();
    // raw json editing
    form_.tabRawComponent->setModel(&dataModel_);
    form_.tabRawComponent->setSelectionModel(selectionModel);
    // read-only triple container
    form_.tabTripleContainer->setModel(&dataModel_);
    form_.tabTripleContainer->setSelectionModel(selectionModel);
    // triple vector
    form_.tabTripleVector->setModel(&dataModel_);
    form_.tabTripleVector->setSelectionModel(selectionModel);
    // property maps
    form_.tabTriplePropertyMap->setModel(&dataModel_);
    form_.tabTriplePropertyMap->setSelectionModel(selectionModel);

    // connection to hide/show tabs depending on selected component type
    connect(form_.tabRawComponent, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);
    connect(form_.tabTripleContainer, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);
    connect(form_.tabTripleVector, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);
    connect(form_.tabTriplePropertyMap, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);


    // connect incoming updates with the history
    connect(&dataModel_, &ECModel::gotEntryAdd, this,
            [this](const ModelEntry& entry)
            {
                this->logUpdate(entry, "ADD");
            });

    connect(&dataModel_, &ECModel::gotEntryRemove, this,
            [this](const ModelEntry& entry)
            {
                this->logUpdate(entry, "REM");
            });

    connect(&dataModel_, &ECModel::gotEntryUpdate, this,
            [this](const ModelEntry& entry)
            {
                this->logUpdate(entry, "UPD");
            });

    // hide all tabs in the beginning, as no data is selected yet.
    updateTabStatus(form_.tabRawComponent, false);
    updateTabStatus(form_.tabTripleContainer, false);
    updateTabStatus(form_.tabTriplePropertyMap, false);
    updateTabStatus(form_.tabTripleVector, false);

    // connect the checkboxes to show/hide the editors and the map
    connect(form_.boxShowEditors, &QCheckBox::stateChanged,
            form_.tabWidget, &QWidget::setVisible);
    connect(form_.boxShowOSM, &QCheckBox::stateChanged,
            form_.geoMapWidget, &QWidget::setVisible);
}


SemprGui::~SemprGui()
{
}


void SemprGui::logUpdate(const ModelEntry& entry, const QString& mod)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();

    item->setText(0, mod);
    item->setText(1, QString::fromStdString(entry.entityId_));
    item->setText(2, QString::fromStdString(entry.componentId_));

    form_.historyList->insertTopLevelItem(0, item);
}

void SemprGui::updateTabStatus(UsefulWidget* widget, bool visible)
{
    if (visible)
    {
        int index = form_.tabWidget->indexOf(widget);
        if (index == -1)
        {
            int index = form_.tabWidget->addTab(widget, widget->windowTitle());
            form_.tabWidget->setCurrentIndex(index);
        }
    }
    else
    {
        int index = form_.tabWidget->indexOf(widget);
        if (index != -1)
        {
            form_.tabWidget->removeTab(index);
        }
    }
}


}}
