#include "TCPConnectionClient.hpp"
#include "TCPConnectionRequest.hpp"
#include "ECDataZMQ.hpp"

#include <iostream>
#include <exception>

namespace sempr { namespace gui {

TCPConnectionClient::TCPConnectionClient()
    : updateSubscriber_(context_, zmqpp::socket_type::subscribe),
      requestSocket_(context_, zmqpp::socket_type::request),
      running_(false)
{
}


void TCPConnectionClient::connect(
        const std::string& updateEndpoint,
        const std::string& requestEndpoint)
{
    updateSubscriber_.connect(updateEndpoint);
    updateSubscriber_.subscribe("");

    requestSocket_.connect(requestEndpoint);
}


void TCPConnectionClient::start()
{
    running_ = true;

    updateWorker_ = std::thread(
        [this]()
        {
            while (running_)
            {
                zmqpp::message msg;
                bool msgAvailable = updateSubscriber_.receive(msg, true);
                if (msgAvailable)
                {
                    ECData data;
                    Notification action;
                    msg >> data >> action;

                    this->triggerCallback(data, action);

                    /*
                    std::cout << "got an update!" << std::endl
                              << data.entityId << " - " << data.componentId << std::endl
                              << data.componentJSON << std::endl
                              << "mutable? " << data.isComponentMutable << std::endl
                              << "action: " << action
                              << std::endl << std::endl;
                    */
                }
                else
                {
                    // only sleep if there is no message available
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
            std::cout << "worker stopped" << std::endl;
        }
    );
}

void TCPConnectionClient::stop()
{
    running_ = false; // will stop the updateWorker
    if (updateWorker_.joinable()) updateWorker_.join(); // wait for the thread to finish
}


TCPConnectionResponse TCPConnectionClient::execRequest(const TCPConnectionRequest& request)
{
    zmqpp::message reqMsg, resMsg;
    reqMsg << request;

    std::cout << "sending request..." << std::endl;
    requestSocket_.send(reqMsg);
    std::cout << "receiving reply..." << std::endl;
    requestSocket_.receive(resMsg);
    std::cout << "got reply." << std::endl;

    TCPConnectionResponse response;
    resMsg >> response;
    return response;
}


std::vector<ECData> TCPConnectionClient::listEntityComponentPairs()
{
    TCPConnectionRequest request;
    request.action = TCPConnectionRequest::LIST_ALL_EC_PAIRS;

    auto response = execRequest(request);

    if (response.success)
    {
        return response.data;
    }
    else
    {
        throw std::runtime_error(response.msg); // TODO better exceptions...
    }
}

void TCPConnectionClient::addEntityComponentPair(const ECData& data)
{
    TCPConnectionRequest request;
    request.action = TCPConnectionRequest::ADD_EC_PAIR;
    request.data = data;

    auto response = execRequest(request);
    if (!response.success) throw std::runtime_error(response.msg);
}

void TCPConnectionClient::modifyEntityComponentPair(const ECData& data)
{
    TCPConnectionRequest request;
    request.action = TCPConnectionRequest::MODIFY_EC_PAIR;
    request.data = data;

    auto response = execRequest(request);
    if (!response.success) throw std::runtime_error(response.msg);
}

void TCPConnectionClient::removeEntityComponentPair(const ECData& data)
{
    TCPConnectionRequest request;
    request.action = TCPConnectionRequest::REMOVE_EC_PAIR;
    request.data = data;

    auto response = execRequest(request);
    if (!response.success) throw std::runtime_error(response.msg);
}

}}
