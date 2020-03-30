#include "SemprGui.hpp"

namespace sempr { namespace gui {

SemprGui::SemprGui(AbstractInterface::Ptr interface)
    : dataModel_(interface)
{
    form_.setupUi(this);
    // both views use the same model
    form_.treeView->setModel(&dataModel_);
    form_.tabRawComponent->setModel(&dataModel_);
    form_.tabRawComponent->setSelectionModel(form_.treeView->selectionModel());

    form_.tabTriplePropertyMap->setModel(&dataModel_);
    form_.tabTriplePropertyMap->setSelectionModel(form_.treeView->selectionModel());

    connect(form_.tabRawComponent, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);
    connect(form_.tabTriplePropertyMap, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);

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
}


SemprGui::~SemprGui()
{
}


void SemprGui::logUpdate(const ModelEntry& entry, const QString& mod)
{
    QListWidgetItem* item = new QListWidgetItem();
    QString text = "%1 | %2 | %3";
    text = text.arg(mod);

    item->setText(
        text.arg(QString::fromStdString(entry.entityId_))
            .arg(QString::fromStdString(entry.componentId_))
    );

    form_.historyList->insertItem(0, item);
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
