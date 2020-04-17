#include <iostream>
#include "TCPConnectionClient.hpp"
#include <thread>
#include <chrono>

int main()
{
    sempr::gui::TCPConnectionClient client;
    client.connect("tcp://localhost:4242", "tcp://localhost:4243");
    client.start();

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}
