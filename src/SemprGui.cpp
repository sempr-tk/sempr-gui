#include "SemprGui.hpp"

namespace sempr { namespace gui {

SemprGui::SemprGui(AbstractInterface::Ptr interface)
    : dataModel_(interface)
{
    form_.setupUi(this);
    // both views use the same model
    form_.treeView->setModel(&dataModel_);
    form_.widget->setModel(&dataModel_);
    form_.widget->setSelectionModel(form_.treeView->selectionModel());
}


SemprGui::~SemprGui()
{
}


}}
