#ifndef SEMPR_GUI_ECDATAZMQ_HPP_
#define SEMPR_GUI_ECDATAZMQ_HPP_

#include "AbstractInterface.hpp"
#include <zmqpp/zmqpp.hpp>

namespace sempr { namespace gui {


    // pushing an ECData package into a message
    inline zmqpp::message& operator << (zmqpp::message& msg, const ECData& data)
    {
        msg << data.entityId << data.componentId
            << data.componentJSON << data.tag
            << data.isComponentMutable;
        return msg;
    }

    // getting an ECData package from a message
    inline zmqpp::message& operator >> (zmqpp::message& msg, ECData& data)
    {
        msg >> data.entityId >> data.componentId
            >> data.componentJSON >> data.tag
            >> data.isComponentMutable;
        return msg;
    }

    // pushing an AbtractInterface::Notification into a message
    inline zmqpp::message& operator << (zmqpp::message& msg, AbstractInterface::Notification data)
    {
        switch (data) {
            case AbstractInterface::Notification::ADDED:
                msg << "ADDED"; break;
            case AbstractInterface::Notification::REMOVED:
                msg << "REMOVED"; break;
            case AbstractInterface::Notification::UPDATED:
                msg << "UPDATED"; break;
        }

        return msg;
    }

    // getting an AbstractInterface::Notification from a message
    inline zmqpp::message& operator >> (zmqpp::message& msg, AbstractInterface::Notification& data)
    {
        std::string n;
        msg >> n;
        if (n == "ADDED") data = AbstractInterface::Notification::ADDED;
        else if (n == "REMOVED") data = AbstractInterface::Notification::REMOVED;
        else if (n == "UPDATED") data = AbstractInterface::Notification::UPDATED;
        else throw std::exception();

        return msg;
    }

}}

#endif /* include guard: SEMPR_GUI_ECDATAZMQ_HPP_ */
