#ifndef SEMPR_GUI_ABSTRACTINTERFACE_HPP_
#define SEMPR_GUI_ABSTRACTINTERFACE_HPP_

#include <QtCore>

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <mutex>
#include <chrono>

#include "ReteVisualSerialization.hpp"
#include "Rule.hpp"
#include <sempr/component/TripleContainer.hpp> // for sempr::Triple
#include <sempr/ECWME.hpp>
#include "ExplanationNode.hpp"

namespace sempr { namespace gui {

/**
    This ECData struct is the data exchange format for the AbstractInterface.
    It contains the minimum information that needs to be known/exchanged:
    Identifiers for the entity and component, the JSON representation of the
    component, and a flag representing if the component is mutable or should
    not be changed as it is the result of some reasoning procedure.

    This is very similar to the ModelEntry class used in the basic data model,
    but still separated from it to cleanly distinguish between the GUI and
    data-exchange parts/responsibilities. Most importantly, it does not hold
    any object pointers, which prevents an implementation of AbstractInterface
    to take wrong shortcuts. (Same for the gui part).
*/
struct ECData {
    std::string entityId;
    std::string componentId;
    std::string componentJSON;
    bool isComponentMutable;
};


/**
    A struct carrying information about any kind of error/exception/warning/..
    which does not belong to the actual data but arises while processing them
    etc.
*/
struct LogData {
    typedef std::chrono::time_point<
                std::chrono::system_clock,
                std::chrono::system_clock::duration> sys_time;

    enum Level { DEBUG, INFO, WARNING, ERROR };
    Level level;            // log category
    std::string name;       // (short) general name/type of error/...
    std::string message;    // (longer) specific description what happened
    sys_time timestamp;     // when did this happen?
};


/**
    The AbstractInterface specifies the ways in which the sempr core can be
    accessed. The GUI uses this interface to retrieve, modify, add and remove
    data from the sempr instance it is connected to. An implementation of this
    interface might directly access the sempr core or connect to it through
    some other transport layer.

    The general idea behind this interface is to only send update-requests to
    the core, but not expect any response. Those are sent asynchronously, and
    without a way to associate it with a previous request. This has some
    limitations, but makes a very small, clean interface with little room for
    errors: All updates to the GUI are provided through the callback function,
    and this is triggered by a rule activation in the cores reasoner. Hence,
    whatever one GUI instance does, whatever any other component does, the
    necessary updates are sent to all connected GUIs.

    The only sad thing about this is that it is very hard to catch any errors.
*/
class AbstractInterface {
public:
    using Ptr = std::shared_ptr<AbstractInterface>;
    virtual ~AbstractInterface() = default;

    // for the callback
    enum Notification { ADDED, UPDATED, REMOVED };
    typedef std::function<void(ECData, Notification)> callback_t;
    typedef std::function<void(sempr::Triple, Notification)> triple_callback_t;
    typedef std::function<void(LogData)> logging_callback_t;


    /**
        Returns a simplified representation of the internal rete network --
        just IDs with labels.
    */
    virtual Graph getReteNetworkRepresentation() = 0;

    /**
        Returns a simplified representation of an explanation -- again,
        basically ids with labels.
    */
    virtual ExplanationGraph getExplanation(sempr::Triple::Ptr triple) = 0;
    virtual ExplanationGraph getExplanation(const ECData& ec) = 0;

    /**
        Returns a list of the currently implemented rules:
        Textual representation + a list of node IDs which implement the
        rules effects.
    */
    virtual std::vector<Rule> getRulesRepresentation() = 0;

    /**
        Lists all entity-component pairs present in the reasoner.
    */
    virtual std::vector<ECData> listEntityComponentPairs() = 0;

    /**
        Lists all triples present in the reasoner
    */
    virtual std::vector<sempr::Triple> listTriples() = 0;

    /**
        Adds a new component to the entity. The only relevant parameters are
        the entityId and componentJSON -- the component will be mutable by
        construction, and the componentId will be chosen by the core.
        There is no return value -- the core only sends updates that are
        then handled in the set callback function.
    */
    virtual void addEntityComponentPair(const ECData&) = 0;

    /**
        Sends an update to the component of an entity.
        Requires entityId, componentId and componentJSON to be set.
    */
    virtual void modifyEntityComponentPair(const ECData&) = 0;

    /**
        Triggers removal of a specified component from an entity.
        Requires only entityId and componentId to be set.
    */
    virtual void removeEntityComponentPair(const ECData&) = 0;

    /**
        Sets a callback that is triggered whenever an entity-component-pair
        in the core changes
    */
    void setUpdateCallback(callback_t);
    void setTripleUpdateCallback(triple_callback_t);
    void setLoggingCallback(logging_callback_t);

    /**
        Removes the currently set callback
    */
    void clearUpdateCallback();
    void clearTripleUpdateCallback();
    void clearLoggingCallback();

    /**
        Calls the internally stored callback
    */
    void triggerCallback(callback_t::first_argument_type,
                         callback_t::second_argument_type);

    void triggerTripleCallback(triple_callback_t::first_argument_type,
                               triple_callback_t::second_argument_type);

    void triggerLoggingCallback(logging_callback_t::argument_type);

private:
    std::mutex callbackMutex_;
    callback_t callback_;
    triple_callback_t tripleCallback_;
    logging_callback_t loggingCallback_;
};


}}

Q_DECLARE_METATYPE(sempr::gui::ECData)

#endif /* include guard: SEMPR_GUI_ABSTRACTINTERFACE_HPP_ */
