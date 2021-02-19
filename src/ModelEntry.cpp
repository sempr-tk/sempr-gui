#include "ModelEntry.hpp"

#include <sempr/Component.hpp>
#include <cereal/archives/json.hpp>
#include <sstream>

namespace sempr { namespace gui {

ModelEntry::ModelEntry(const ECData& data)
{
    coreData_ = data;
    setJSON(data.componentJSON);
    setTag(data.tag);
}

std::string ModelEntry::entityId()           const { return coreData_.entityId; }
std::string ModelEntry::componentId()        const { return coreData_.componentId; }
std::string ModelEntry::tag()                const { return tag_; }
bool        ModelEntry::isComponentMutable() const { return coreData_.isComponentMutable; }

void ModelEntry::setTag(const std::string& tag)
{
    this->tag_ = tag;
}

bool ModelEntry::isModified() const
{
    return !(coreData_.componentJSON == componentJSON_) ||
           !(coreData_.tag == tag_);
}


std::string ModelEntry::json() const
{
    return componentJSON_;
}

Component::Ptr ModelEntry::component() const
{
    return component_;
}

bool ModelEntry::setJSON(const std::string& json)
{
    componentJSON_ = json;
    try {
        std::stringstream ss(json);
        cereal::JSONInputArchive ar(ss);
        ar(component_);
    } catch (cereal::Exception& e) {
        std::cerr << "ModelEntry::setJSON: " << e.what() << std::endl;
        component_ = nullptr;
        //return false;
        // let the ECModel handle this.
        throw;
    }

    return true;
}


void ModelEntry::componentPtrChanged()
{
    if (!component_) return;

    try {
        std::stringstream ss;
        {
            cereal::JSONOutputArchive ar(ss);
            ar(component_);
        }
        componentJSON_ = ss.str();
    } catch (cereal::Exception& e) {
        std::cerr << "ModelEntry::setJSON: " << e.what() << std::endl;
        componentJSON_ = "";
        // let the ECModel handle this.
        throw;
    }
}

}}
