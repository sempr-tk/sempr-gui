#include "TCPConnectionClient.hpp"
#include "TCPConnectionRequest.hpp"
#include "ECDataZMQ.hpp"
#include "LogDataZMQ.hpp"

#include <cereal/archives/json.hpp>
#include <iostream>
#include <exception>

namespace sempr { namespace gui {

TCPConnectionClient::TCPConnectionClient()
    : updateSubscriber_(context_, zmqpp::socket_type::subscribe),
      requestSocket_(context_, zmqpp::socket_type::request),
      loggingSubscriber_(context_, zmqpp::socket_type::subscribe),
      running_(false)
{
}


void TCPConnectionClient::connect(
        const std::string& updateEndpoint,
        const std::string& requestEndpoint)
{
    updateSubscriber_.connect(updateEndpoint);
    updateSubscriber_.subscribe("data");

    loggingSubscriber_.connect(updateEndpoint);
    loggingSubscriber_.subscribe("logging");

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
                std::string topic;
                bool msgAvailable = updateSubscriber_.receive(topic, true);
                if (msgAvailable)
                {
                    updateSubscriber_.receive(msg);

                    UpdateType type;
                    msg >> type;
                    if (type == UpdateType::EntityComponent)
                    {
                        ECData data;
                        Notification action;

                        msg >> data >> action;

                        this->triggerCallback(data, action);
                    }
                    else if (type == UpdateType::Triple)
                    {
                        std::cout << "TCPConnectionClient - trigger triple update callback" << std::endl;

                        std::string tripleString;
                        Notification action;

                        msg >> tripleString >> action;

                        std::stringstream ss(tripleString);
                        cereal::JSONInputArchive ar(ss);
                        sempr::Triple triple;
                        ar(triple);

                        this->triggerTripleCallback(triple, action);
                    }
                    else
                    {
                        std::cerr << "unknown update message type"
                                  << static_cast<int>(type) << std::endl;
                    }
                }

                bool logAvailable = loggingSubscriber_.receive(topic, true);
                if (logAvailable)
                {
                    loggingSubscriber_.receive(msg);

                    LogData log;
                    msg >> log;
                    this->triggerLoggingCallback(log);
                }

                if (!(msgAvailable || logAvailable))
                {
                    // only sleep if there is no message available
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
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

    requestSocket_.send(reqMsg);
    requestSocket_.receive(resMsg);

    TCPConnectionResponse response;
    resMsg >> response;
    return response;
}


Graph TCPConnectionClient::getReteNetworkRepresentation()
{
    TCPConnectionRequest request;
    request.action = TCPConnectionRequest::GET_RETE_NETWORK;
    auto response = execRequest(request);

    if (response.success)
    {
        std::stringstream ss(response.reteNetwork);
        cereal::JSONInputArchive ar(ss);

        Graph g;
        ar >> g;

        return g;
    }
    else
    {
        throw std::runtime_error(response.msg); // TODO better exceptions...
    }
}

ExplanationGraph TCPConnectionClient::getExplanation(const ECData& ec)
{
    TCPConnectionRequest request;
    request.action = TCPConnectionRequest::GET_EXPLANATION_ECWME;
    request.data = ec;

    auto response = execRequest(request);

    if (response.success)
    {
        return response.explanationGraph;
    }
    else
    {
        throw std::runtime_error(response.msg); // TODO better exceptions...
    }
}

ExplanationGraph TCPConnectionClient::getExplanation(sempr::Triple::Ptr triple)
{
    TCPConnectionRequest request;
    request.action = TCPConnectionRequest::GET_EXPLANATION_TRIPLE;
    request.toExplain = *triple;

    auto response = execRequest(request);

    if (response.success)
    {
        return response.explanationGraph;
    }
    else
    {
        throw std::runtime_error(response.msg); // TODO better exceptions...
    }
}


std::vector<Rule> TCPConnectionClient::getRulesRepresentation()
{
    TCPConnectionRequest request;
    request.action = TCPConnectionRequest::GET_RULES;
    auto response = execRequest(request);

    if (response.success)
    {
        return response.rules;
    }
    else
    {
        throw std::runtime_error(response.msg); // TODO better exceptions...
    }
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

std::vector<sempr::Triple> TCPConnectionClient::listTriples()
{
    TCPConnectionRequest request;
    request.action = TCPConnectionRequest::LIST_ALL_TRIPLES;
    auto response = execRequest(request);

    if (response.success)
    {
        return response.triples;
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
