#include <iostream>
#include <thread>
#include "server.h"
#include "Client.h"

int main()
{
    Server server;
    Client client;
    std::thread serverMainThread(&Server::run, &server);
    serverMainThread.detach();
    //std::thread serverMainThread(serverMain);
    client.run();
    return 0;
}
