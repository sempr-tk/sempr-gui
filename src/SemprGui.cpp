#include "SemprGui.hpp"

namespace sempr { namespace gui {

SemprGui::SemprGui(AbstractInterface::Ptr interface)
    : semprInterface_(interface)
{
    form_.setupUi(this);
    // both views use the same model
    form_.treeView->setModel(&dataModel_);
    form_.listView->setModel(&dataModel_);

    // and share the same selection
    form_.treeView->setSelectionModel(form_.listView->selectionModel());
}


SemprGui::~SemprGui()
{
}


}}
