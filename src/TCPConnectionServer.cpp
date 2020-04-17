#include "TCPConnectionServer.hpp"
#include "ECDataZMQ.hpp"

namespace sempr { namespace gui {

TCPConnectionServer::TCPConnectionServer(
        DirectConnection::Ptr con,
        const std::string& publishEndpoint,
        const std::string& requestEndpoint)
    :
        updatePublisher_(context_, zmqpp::socket_type::publish),
        replySocket_(context_, zmqpp::socket_type::reply),
        semprConnection_(con)
{
    updatePublisher_.bind(publishEndpoint);
    replySocket_.bind(requestEndpoint);

    con->setUpdateCallback(
        std::bind(
            &TCPConnectionServer::updateCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}


void TCPConnectionServer::updateCallback(
        AbstractInterface::callback_t::first_argument_type data,
        AbstractInterface::callback_t::second_argument_type action)
{
    // construct the message -- just all the data entries in the ECData struct,
    // plus the action.
    zmqpp::message msg;
    msg << data << action;

    // and send it to all subscribers
    updatePublisher_.send(msg);
}

}}
