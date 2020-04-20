#include <iostream>
#include "TCPConnectionClient.hpp"
#include <thread>
#include <chrono>

#include "SemprGui.hpp"
#include <QtCore>
#include <QApplication>

int main(int argc, char** args)
{
    auto client = std::make_shared<sempr::gui::TCPConnectionClient>();
    client->connect("tcp://localhost:4242", "tcp://localhost:4243");
    client->start();

    std::cout << "started client" << std::endl;

    QApplication app(argc, args);

    std::cout << "created app" << std::endl;

    sempr::gui::SemprGui gui(client);

    std::cout << "created gui" << std::endl;

    gui.show();

    std::cout << "showing gui, going into exec() loop" << std::endl;

    app.exec();
}
