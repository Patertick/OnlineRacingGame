#include "receiver.h"
#include "util.h"
#include <chrono>
#include <cstring>
#include <iostream>
#include <iostream>
#include <sstream>
#include <thread>
#include "Message.h"

Receiver::Receiver(std::shared_ptr<sf::TcpSocket> s,
    Queue<Message>& q) :
    socket_(s),
    queue_(q)
{
}

void Receiver::recv_loop()
{
    char buffer[256];
    {
        std::stringstream ss;
        ss << socket_->getRemoteAddress() << ":" << socket_->getRemotePort() << std::endl;
        std::cout << ss.str();
    }
    while (1)
    {
        sf::Packet packet;
        Message msg;
        sf::Socket::Status status = socket_->receive(packet);
        if (status == sf::Socket::Status::Done)
        {
            packet >> msg;
            std::stringstream ss;
            ss << "Received: \"" << msg.posX << "\" from " << msg.ID << "." << std::endl;
            //std::cout << ss.str();
        }
        queue_.push(msg);
    }
}