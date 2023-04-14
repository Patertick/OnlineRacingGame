#include "accepter.h"
#include "receiver.h"
#include "util.h"
#include "Message.h"
#include <iostream>
#include <sstream>
#include <thread>


Accepter::Accepter(Queue<Message>& q, List<std::shared_ptr<sf::TcpSocket>>& s) :
    queue_(q),
    socket_(s)
{}

void Accepter::operator()()
{
    sf::TcpListener listener;
    // TODO the listener has to listen. DONE
    sf::Socket::Status status = listener.listen(PORT);
    if (status != sf::Socket::Status::Done)
    {
        std::cout << "Failed to listen" << std::endl;
        return;
    }
    std::cout << "Bound to port\n";
    while (1)
    {

        std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();
        // TODO accept a connection on socket DONE
        status = listener.accept(*socket);
        if (status == sf::Socket::Status::Done)
        {
            socket_.push(socket);
            std::stringstream ss;
            ss << "Accepted a connection from: "
                << socket->getRemoteAddress()
                << ":"
                << socket->getRemotePort()
                << std::endl;
            std::cout << ss.str();
            std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(socket, queue_);
            // TODO launch a thread to receive with the receiver DONE
            std::thread receiverThread(&Receiver::recv_loop, receiver);

            receiverThread.detach();
        }
    }
}

