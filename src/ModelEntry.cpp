#include "ModelEntry.hpp"

#include <sempr/Component.hpp>
#include <cereal/archives/json.hpp>
#include <sstream>

namespace sempr { namespace gui {

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
        return false;
    }

    return true;
}


bool ModelEntry::setComponent(Component::Ptr c)
{
    component_ = c;
    try {
        std::stringstream ss;
        {
            cereal::JSONOutputArchive ar(ss);
            ar(c);
        }
        componentJSON_ = ss.str();
    } catch (cereal::Exception& e) {
        std::cerr << "ModelEntry::setJSON: " << e.what() << std::endl;
        componentJSON_ = "";
        return false;
    }

    return true;
}

}}
