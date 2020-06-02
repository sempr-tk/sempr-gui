#ifndef SEMPR_GUI_TCPCONNECTIONCLIENT_HPP_
#define SEMPR_GUI_TCPCONNECTIONCLIENT_HPP_

#include "AbstractInterface.hpp"
#include "TCPConnectionRequest.hpp"

#include <zmqpp/zmqpp.hpp>
#include <thread>
#include <atomic>

namespace sempr { namespace gui {


/**
    This is the client-side of the TCPConnection, an implementation of the
    AbstractInterface. Instead of directly accessing a sempr instance, it
    connects to a TCPConnectionServer.
*/
class TCPConnectionClient : public AbstractInterface {
    zmqpp::context context_;
    zmqpp::socket updateSubscriber_;
    zmqpp::socket requestSocket_;

    // an extra thread handling messages incoming on updateSubscriber_
    std::thread updateWorker_;
    std::atomic<bool> running_;

    // convenience method to execute a request and get a response
    TCPConnectionResponse execRequest(const TCPConnectionRequest&);
public:
    using Ptr = std::shared_ptr<TCPConnectionClient>;
    TCPConnectionClient();

    // creates a connection to the server
    void connect(const std::string& updateEndpoint,
                 const std::string& requestEndpoint);

    // handling updates in a separate thread
    void start();
    void stop();

    Graph getReteNetworkRepresentation() override;
    ExplanationGraph getExplanation(const ECData &ec) override;
    ExplanationGraph getExplanation(sempr::Triple::Ptr triple) override;
    std::vector<Rule> getRulesRepresentation() override;
    std::vector<ECData> listEntityComponentPairs() override;
    std::vector<sempr::Triple> listTriples() override;
    void addEntityComponentPair(const ECData&) override;
    void modifyEntityComponentPair(const ECData&) override;
    void removeEntityComponentPair(const ECData&) override;
};


}}

#endif /* include guard: SEMPR_GUI_TCPCONNECTIONCLIENT_HPP_ */
