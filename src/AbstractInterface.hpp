#ifndef SEMPR_GUI_ABSTRACTINTERFACE_HPP_
#define SEMPR_GUI_ABSTRACTINTERFACE_HPP_

#include <vector>
#include <string>
#include <functional>
#include <memory>

#include "ModelEntry.hpp"

namespace sempr { namespace gui {



/**
    The AbstractInterface specifies the ways in which the sempr core can be
    accessed. The GUI uses this interface to retrieve, modify, add and remove
    data from the sempr instance it is connected to. An implementation of this
    interface might directly access the sempr core or connect to it through
    some other transport layer.

    The data exchange format in the interface it the ModelEntry struct. This
    contains redundant information: A Component::Ptr as well as a JSON string
    representing the component. Make sure that the ModelEntry is consistent when
    handing it as an argument to an interface method, since the actual
    implementation may use either of the json or the ptr.
*/
class AbstractInterface {
public:
    using Ptr = std::shared_ptr<AbstractInterface>;
    virtual ~AbstractInterface() = default;

    // for the callback
    enum Notification { ADDED, UPDATED, REMOVED };
    typedef std::function<void(ModelEntry, Notification)> callback_t;

    /**
        Lists all entity-component pairs present in the reasoner.
    */
    virtual std::vector<ModelEntry> listEntityComponentPairs() = 0;

    /**
        Adds a new component to the entity.
        Returns the id that has been assigned to the component by the core.
    */
    virtual std::string addEntityComponentPair(const ModelEntry&) = 0;

    /**
        Sends an update to the component of an entity
    */
    virtual void modifyEntityComponentPair(const ModelEntry&) = 0;

    /**
        Triggers removal of a specified component from an entity
    */
    virtual void removeEntityComponentPair(const ModelEntry&) = 0;

    /**
        Sets a callback that is triggered whenever an entity-component-pair
        in the core changes
    */
    virtual void setUpdateCallback(callback_t) = 0;

    /**
        Removes the currently set callback
    */
    virtual void clearUpdateCallback() = 0;

    /**
        Calls the internally stored callback
    */
    virtual void triggerCallback(callback_t::first_argument_type,
                                 callback_t::second_argument_type) = 0;
};


}}

#endif /* include guard: SEMPR_GUI_ABSTRACTINTERFACE_HPP_ */
