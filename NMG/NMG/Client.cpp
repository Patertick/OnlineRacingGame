#include "queue.h"
#include "Client.h"
#include "receiver.h"
#include "util.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

int Client::run()
{
    std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();
    // auto connect = [&] { return socket->connect(sf::IpAddress::getLocalAddress(), PORT); };
    // net_run(connect, "connect");
    sf::Socket::Status status = socket->connect(sf::IpAddress::getLocalAddress(), PORT);
    if (status != sf::Socket::Done) {
        return 1;
    }
    std::cout << "Connected\n";
    Queue<std::string> queue;
    // TODO launch a receiver thread to receive messages from the server. DONE
    std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(socket, queue);
    std::thread receiverThread(&Receiver::recv_loop, receiver);


    std::string s = "";
    while (1)
    {
        std::getline(std::cin, s);
        std::cout << "Sending: \"" << s << "\"" << std::endl;
        // TODO send messages to the server DONE
        status = socket->send(s.c_str(), s.size() + 1);
        if (status != sf::Socket::Status::Done)
        {
            std::cout << "Failed to send" << std::endl;
        }
    }
    receiverThread.join();
    return 0;
}