#include "accepter.h"
#include "server.h"
#include "queue.h"
#include "list.h"
#include "receiver.h"
#include "util.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>


int Server::run()
{
    Queue<std::string> queue;
    List<std::shared_ptr<sf::TcpSocket>> sockets;
    // TODO launch an accepter thread. DONE
    Accepter accepter(queue, sockets);
    std::thread accepterThread(&Accepter::operator(), &accepter);
    while (1)
    {
        std::string s = queue.pop();
        std::cout << "Main read: \"" << s << "\"\n";
        // TODO send to all in sockets. Be careful to synchronise. DONE
//        for(std::list<std::shared_ptr<sf::TcpSocket>>::iterator itr = sockets.begin(); itr != sockets.end(); ++itr)
//        {
//            sf::Socket::Status status = itr->send(s.c_str(), s.size());
//        }
        auto sendToOne = [&s](std::shared_ptr<sf::TcpSocket> socket) {
            socket->send(s.c_str(), s.size() + 1);
        };
        sockets.for_each(sendToOne);
    }
    accepterThread.join();
    return 0;
}
