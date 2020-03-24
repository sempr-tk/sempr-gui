#ifndef SEMPR_GUI_ABSTRACTINTERFACE_HPP_
#define SEMPR_GUI_ABSTRACTINTERFACE_HPP_

#include <vector>
#include <string>

namespace sempr { namespace gui {


/**
    The AbstractInterface specifies the ways in which the sempr core can be
    accessed. The GUI uses this interface to retrieve, modify, add and remove
    data from the sempr instance it is connected to. An implementation of this
    interface might directly access the sempr core or connect to it through
    some other transport layer.
*/
class AbstractInterface {
public:
    /**
        Lists all entity-component pairs present in the reasoner.
    */
    void listEntityComponentPairs();

    /**
        Adds a new component to the entity
    */
    void addEntityComponentPair();

    /**
        Sends an update to the component of an entity
    */
    void modifyEntityComponentPair();

    /**
        Triggers removal of a specified component from an entity
    */
    void removeEntityComponentPair();

    /**
        Sets a callback that is triggered whenever an entity-component-pair
        in the core changes
    */
    void setUpdateCallback();
};


}}

#endif /* include guard: SEMPR_GUI_ABSTRACTINTERFACE_HPP_ */
