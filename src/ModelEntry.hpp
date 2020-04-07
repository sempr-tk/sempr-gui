#ifndef SEMPR_GUI_MODELENTRY_HPP_
#define SEMPR_GUI_MODELENTRY_HPP_

#include <QtCore>
#include <string>

#include <sempr/Component.hpp>
#include <cereal/external/rapidjson/document.h>

#include "AbstractInterface.hpp"

namespace sempr { namespace gui {

/**
    The ModelEntry holds all the data that is stored in the QAbstractItemModel
    (ECModel) for the views in the GUI. This is basically the Data as it was
    retrieved by the core, as well as a copy of the data in form of a JSON
    string and a deserialized Component::Ptr as a "staging" area. The different
    views/editors may edit these, and the state can be reset if mistakes were
    made.
*/
class ModelEntry {

    /**
        The data as it was retrieved by the core.
    */
    ECData coreData_;

    /**
        A copy of the json representation of the component.
        Even if the GUI is not aware of the concrete component type
        (and thus the deserialization fails), we can still view and edit the
        raw json.
    */
    std::string componentJSON_;

    /**
        A Component::Ptr deserialized from the componentJSON_, for convenience.
    */
    Component::Ptr component_;

    friend class ECModel;
protected:
    /**
        Constructs a ModelEntry from some received data.
        This already tries to deserialize the json representation to set the
        Component::Ptr.
    */
    explicit ModelEntry(const ECData& data);

public:
    ModelEntry() = default;
    ModelEntry(const ModelEntry&) = default;
    ~ModelEntry() = default;

    std::string entityId() const;
    std::string componentId() const;
    bool isComponentMutable() const;


    /**
        Compares the current json representation with the one originally
        received from the sempr core to tell if this entry has been modified
        locally and needs to be committed.
    */
    bool isModified() const;

    /**
        Returns the current state of the model entry in json format.
    */
    std::string json() const;

    /**
        Returns the current state of the model entry as a component ptr.
    */
    Component::Ptr component() const;

    /**
        Sets componentJSON_ = json and tries to de-serialize the component.
        On success, sets component_ to the newly created component, returns true.
        On failure, sets component_ to nullptr, returns false;
    */
    bool setJSON(const std::string& json);


    /**
        Updates componentJSON_ by deserializing the component_ pointer again.
    */
    void componentPtrChanged();

};


}}

Q_DECLARE_METATYPE(sempr::gui::ModelEntry)

// also allow Component::Ptr
Q_DECLARE_METATYPE(sempr::Component::Ptr)

#endif /* include guard: SEMPR_GUI_MODELENTRY_HPP_ */
