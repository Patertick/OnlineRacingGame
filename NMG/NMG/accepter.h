#ifndef ACCEPTER_HPP
#define ACCEPTER_HPP

#include "queue.h"
#include "list.h"
#include <SFML/Network.hpp>

struct Message;

class Accepter {
private:
    Queue<Message>& queue_;
    List<std::shared_ptr<sf::TcpSocket>>& socket_;

public:
    Accepter(Queue<Message>& q, List<std::shared_ptr<sf::TcpSocket>>& s);
    void operator()();
};


#endif
