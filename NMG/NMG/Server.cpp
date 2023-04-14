#include "accepter.h"
#include "server.h"
#include "queue.h"
#include "list.h"
#include "receiver.h"
#include "util.h"
#include "Message.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>


int Server::run()
{
    Queue<Message> queue;
    List<std::shared_ptr<sf::TcpSocket>> sockets;
    // TODO launch an accepter thread. DONE
    Accepter accepter(queue, sockets);
    std::thread accepterThread(&Accepter::operator(), &accepter);
    accepterThread.detach();
    while (1)
    {
        Message msg = queue.pop();
        std::cout << "Main read: \"" << msg.posX << "\"\n";

        sf::Packet packet;
        packet << msg;

        auto sendToOne = [&packet](std::shared_ptr<sf::TcpSocket> socket) {
            socket->send(packet);
        };
        sockets.for_each(sendToOne);
    }
    return 0;
}
