#include "TCPConnectionClient.hpp"
#include "ECDataZMQ.hpp"

#include <iostream>

namespace sempr { namespace gui {

TCPConnectionClient::TCPConnectionClient()
    : updateSubscriber_(context_, zmqpp::socket_type::subscribe),
      requestSocket_(context_, zmqpp::socket_type::request)
{
}


void TCPConnectionClient::connect(
        const std::string& updateEndpoint,
        const std::string& requestEndpoint)
{
    updateSubscriber_.connect(updateEndpoint);
    requestSocket_.connect(requestEndpoint);
}


void TCPConnectionClient::start()
{
    updateWorker_ = std::thread(
        [this]()
        {
            while (true)
            {
                zmqpp::message msg;
                updateSubscriber_.receive(msg);
                ECData data;
                Notification action;
                msg >> data >> action;

                std::cout << "got an update!" << std::endl
                          << data.entityId << " - " << data.componentId << std::endl
                          << data.componentJSON << std::endl
                          << "mutable? " << data.isComponentMutable << std::endl
                          << "action: " << action
                          << std::endl << std::endl;
            }
        }
    );
}


std::vector<ECData> TCPConnectionClient::listEntityComponentPairs()
{
    // TODO
}

void TCPConnectionClient::addEntityComponentPair(const ECData& data)
{
    // TODO
}

void TCPConnectionClient::modifyEntityComponentPair(const ECData& data)
{
    // TODO
}

void TCPConnectionClient::removeEntityComponentPair(const ECData& data)
{
    // TODO
}

}}
