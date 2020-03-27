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

    connect(form_.tabRawComponent, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);
}


SemprGui::~SemprGui()
{
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
