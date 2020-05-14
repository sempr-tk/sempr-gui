#ifndef SEMPR_GUI_TCPCONNECTIONREQUEST_HPP_
#define SEMPR_GUI_TCPCONNECTIONREQUEST_HPP_

#include "AbstractInterface.hpp"
#include "ECDataZMQ.hpp"
#include "Rule.hpp"

#include <zmqpp/zmqpp.hpp>
#include <cereal/archives/json.hpp>

namespace sempr { namespace gui {

/**
    A wrapper for a request made from the client side. Translates to the
    AbstractInterface.
*/
struct TCPConnectionRequest {
    enum Action {
        LIST_ALL_EC_PAIRS = 0,
        ADD_EC_PAIR,
        MODIFY_EC_PAIR,
        REMOVE_EC_PAIR,
        GET_RETE_NETWORK,
        GET_RULES
    };

    Action action;
    ECData data;
};


/**
    Well, when there is a "Request" type, there should also be a "Response", right?
    This is just a very crude wrapper. Contains more than it should.
*/
struct TCPConnectionResponse {
    bool success;
    std::string msg; // in case of errors, here could be some description.
    std::vector<ECData> data; // just for the LIST_ALL_EC_PAIRS action, contains all the EC pairs.
    std::string reteNetwork; // just for GET_RETE_NETWORK action, json representation of a Graph
    std::vector<Rule> rules; // just for GET_RULES
};


// helper: write sempr::gui::Rule to the message type
inline zmqpp::message& operator << (zmqpp::message& msg, Rule r)
{
    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar(ss);
        ar(r);
    }

    msg << ss.str();
    return msg;
}

// helper: read sempr::gui::Rule from message type
inline zmqpp::message& operator >> (zmqpp::message& msg, Rule& r)
{
    std::string s;
    msg >> s;

    std::stringstream ss(s);
    cereal::JSONInputArchive ar(ss);
    ar >> r;

    return msg;
}


// next, we need operators to read and write requests and responses from/to
// the zmqpp::message type.

// helper: TCPConnectionRequest::Action enum
inline zmqpp::message& operator << (zmqpp::message& msg, TCPConnectionRequest::Action action)
{
    msg << static_cast<int>(action);
    return msg;
}

inline zmqpp::message& operator >> (zmqpp::message& msg, TCPConnectionRequest::Action& action)
{
    int tmp;
    msg >> tmp;
    action = static_cast<TCPConnectionRequest::Action>(tmp);
    return msg;
}


// write request
inline zmqpp::message& operator << (zmqpp::message& msg, const TCPConnectionRequest& request)
{
    msg << request.data << request.action;
    return msg;
}

// read request
inline zmqpp::message& operator >> (zmqpp::message& msg, TCPConnectionRequest& request)
{
    msg >> request.data >> request.action;
    return msg;
}

// write response
inline zmqpp::message& operator << (zmqpp::message& msg, const TCPConnectionResponse& response)
{
    msg << response.success << response.msg;
    msg << response.data.size();
    for (auto& data : response.data)
    {
        msg << data;
    }
    msg << response.reteNetwork;

    msg << response.rules.size();
    for (auto& rule : response.rules)
    {
        msg << rule;
    }

    return msg;
}

// read response
inline zmqpp::message& operator >> (zmqpp::message& msg, TCPConnectionResponse& response)
{
    msg >> response.success >> response.msg;
    size_t numData;
    msg >> numData;
    response.data.reserve(numData);
    for (size_t i = 0; i < numData; i++)
    {
        ECData d;
        msg >> d;
        response.data.push_back(d);
    }
    msg >> response.reteNetwork;

    size_t numRules;
    msg >> numRules;
    for (size_t i = 0; i < numRules; i++)
    {
        Rule r;
        msg >> r;
        response.rules.push_back(r);
    }

    return msg;
}

}}

#endif /* include guard: SEMPR_GUI_TCPCONNECTIONREQUEST_HPP_ */
