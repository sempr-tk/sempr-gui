#ifndef SEMPR_GUI_TCPCONNECTIONSERVER_HPP_
#define SEMPR_GUI_TCPCONNECTIONSERVER_HPP_

#include <zmqpp/zmqpp.hpp>
#include "DirectConnection.hpp"
#include "TCPConnectionRequest.hpp"

#include <thread>
#include <atomic>

namespace sempr { namespace gui {

/**
    This class takes a DirectConnection and makes it available
    over the network by using ZeroMQ.
*/
class TCPConnectionServer {
    zmqpp::context context_;
    // one socket to publish updates to
    zmqpp::socket updatePublisher_;
    // one socket for explicit requests to modify data
    zmqpp::socket replySocket_;

    DirectConnection::Ptr semprConnection_;

    // the thread in which requests are handled
    std::thread requestHandler_;
    // a signal to stop the request handler
    std::atomic<bool> handlingRequests_;

    /**
        The function called by the DirectConnection when stuff is inferred by
        the reasoner. This will send updates on the updatePublisher.
    */
    void updateCallback(AbstractInterface::callback_t::first_argument_type,
                        AbstractInterface::callback_t::second_argument_type);

    /**
        Handling a request
    */
    TCPConnectionResponse handleRequest(const TCPConnectionRequest& request);

    // helper: get the rete network, serialize it to json
    std::string getReteNetwork();

public:
    TCPConnectionServer(
        DirectConnection::Ptr con,
        const std::string& publishEndpoint = "tcp://*:4242",
        const std::string& requestEndpoint = "tcp://*:4243");

    // starts a new thread that handles incoming requests and connects the
    // update callback
    void start();
    void stop();
};

}}


#endif /* include guard: SEMPR_GUI_TCPCONNECTIONSERVER_HPP_ */
