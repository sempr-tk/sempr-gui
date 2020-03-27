#ifndef SEMPR_GUI_MODELENTRY_HPP_
#define SEMPR_GUI_MODELENTRY_HPP_

#include <QtCore>
#include <string>

#include <sempr/Component.hpp>
#include <cereal/external/rapidjson/document.h>

namespace sempr { namespace gui {

/**
    The ModelEntry represents the single data blobs retrieved from the sempr
    core. Be aware that we do not just serialize and send entities -- that
    would not be enough, as it would not include inferred data. Hence we send
    Entity-Component pairs, matching the ECWMEs in the reasoner.
*/
struct ModelEntry {
    ModelEntry() = default;
    ModelEntry(const ModelEntry&) = default;
    ~ModelEntry() = default;


    /**
        Sets componentJSON_ = json and tries to de-serialize the component.
        On success, sets component_ to the newly created component, returns true.
        On failure, sets component_ to nullptr, returns false;
    */
    bool setJSON(const std::string& json);

    /**
        Sets component_ = component and tries to serialize the component into
        componentJSON_.
        On failure, sets componentJSON_ to an empty string and returns false,
        on success returns true.
    */
    bool setComponent(Component::Ptr component);

    /**
        The ID of the entity this data belongs to.
    */
    std::string entityId_;

    /**
        Some string to identify the component. Necessary when not only reading,
        but also modifying data in the core.
    */
    std::string componentId_;

    /**
        The json representation of the component. This is what we will get from
        the AbstractInterface. Even if the GUI is not aware of the concrete
        component type (and thus the deserialization fails), we can still
        view and edit the raw json.
    */
//    rapidjson::Document componentJSON_;
    std::string componentJSON_;

    /**
        A flag that specifies if the component was manually added to the entity
        or if it was inferred, and must/can therefore not be modified.
    */
    bool mutable_;

    /**
        If the GUI is aware of the component type it will hold it here for
        easier access.
    */
    Component::Ptr component_;
};


}}

Q_DECLARE_METATYPE(sempr::gui::ModelEntry)


#endif /* include guard: SEMPR_GUI_MODELENTRY_HPP_ */
