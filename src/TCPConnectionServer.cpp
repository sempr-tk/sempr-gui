#include "TCPConnectionServer.hpp"
#include "ECDataZMQ.hpp"
#include "TCPConnectionRequest.hpp"

#include <cereal/archives/json.hpp>
#include <iostream>

namespace sempr { namespace gui {

TCPConnectionServer::TCPConnectionServer(
        DirectConnection::Ptr con,
        const std::string& publishEndpoint,
        const std::string& requestEndpoint)
    :
        updatePublisher_(context_, zmqpp::socket_type::publish),
        replySocket_(context_, zmqpp::socket_type::reply),
        semprConnection_(con),
        handlingRequests_(false)
{
    updatePublisher_.bind(publishEndpoint);
    replySocket_.bind(requestEndpoint);
}


void TCPConnectionServer::updateCallback(
        AbstractInterface::callback_t::first_argument_type data,
        AbstractInterface::callback_t::second_argument_type action)
{
    // construct the message -- just all the data entries in the ECData struct,
    // plus the action.
    zmqpp::message msg;
    msg << UpdateType::EntityComponent << data << action;

    // and send it to all subscribers
    updatePublisher_.send(msg);
}

void TCPConnectionServer::tripleUpdateCallback(
        AbstractInterface::triple_callback_t::first_argument_type value,
        AbstractInterface::triple_callback_t::second_argument_type action)
{

    std::cout << "TCPConnectionServer::tripleUpdateCallback" << std::endl;

    zmqpp::message msg;

    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar(ss);
        ar(value);
    }

    msg << UpdateType::Triple << ss.str() << action;

    updatePublisher_.send(msg);
}

void TCPConnectionServer::start()
{
    // connect the update callback
    semprConnection_->setUpdateCallback(
        std::bind(
            &TCPConnectionServer::updateCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );

    semprConnection_->setTripleUpdateCallback(
        std::bind(
            &TCPConnectionServer::tripleUpdateCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );

    // start a thread that handles requests
    handlingRequests_ = true;
    requestHandler_ = std::thread(
        [this]()
        {
            while (handlingRequests_)
            {
                zmqpp::message msg;
                bool requestAvailable = replySocket_.receive(msg, true);
                if (requestAvailable)
                {
                    TCPConnectionRequest request;
                    msg >> request;
                    TCPConnectionResponse response = handleRequest(request);
                    zmqpp::message responseMsg;
                    responseMsg << response;
                    replySocket_.send(responseMsg);
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
    );
}


std::string TCPConnectionServer::getReteNetwork()
{
    auto graph = semprConnection_->getReteNetworkRepresentation();

    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar(ss);
        ar(graph);
    }

    return ss.str();
}

TCPConnectionResponse TCPConnectionServer::handleRequest(const TCPConnectionRequest& request)
{
    TCPConnectionResponse response;
    try {
        // just map directly to the DirectConnection we use here.
        switch(request.action) {
            case TCPConnectionRequest::LIST_ALL_EC_PAIRS:
                response.data = semprConnection_->listEntityComponentPairs();
                break;
            case TCPConnectionRequest::ADD_EC_PAIR:
                semprConnection_->addEntityComponentPair(request.data);
                break;
            case TCPConnectionRequest::MODIFY_EC_PAIR:
                semprConnection_->modifyEntityComponentPair(request.data);
                break;
            case TCPConnectionRequest::REMOVE_EC_PAIR:
                semprConnection_->removeEntityComponentPair(request.data);
                break;
            case TCPConnectionRequest::GET_RETE_NETWORK:
                response.reteNetwork = getReteNetwork();
                break;
            case TCPConnectionRequest::GET_RULES:
                response.rules = semprConnection_->getRulesRepresentation();
                break;
            case TCPConnectionRequest::LIST_ALL_TRIPLES:
                response.triples = semprConnection_->listTriples();
                break;
        }
        response.success = true;
    } catch (std::exception& e) {
        // and in case of exceptions just notify the client
        response.success = false;
        response.msg = e.what();
    }

    return response;
}

}}
