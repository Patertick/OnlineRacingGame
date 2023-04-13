#include <iostream>
#include <thread>
#include "server.h"
#include "Client.h"

int main()
{
    Server server;
    Client client;
    std::thread serverMainThread(&Server::run, &server);
    //std::thread serverMainThread(serverMain);
    while (1)
    {
        client.run();
    }
    serverMainThread.join();
    return 0;
}
