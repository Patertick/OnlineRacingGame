#include "accepter.h"
#include "server.h"
#include "queue.h"
#include "list.h"
#include "receiver.h"
#include "util.h"
#include "Car.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>

using namespace sf;

int Server::run()
{
    Queue<Message> queue;
    List<std::shared_ptr<sf::TcpSocket>> sockets;
    // TODO launch an accepter thread. DONE
    Accepter accepter(queue, sockets);
    std::thread accepterThread(&Accepter::operator(), &accepter);

    bool startGame{ false };
    while (startGame == false)
    {
        if (sockets.Length() > 0)
        {
            startGame = true;
        }
        std::cout << "Number of players in lobby: " << sockets.Length() << std::endl;
    }
    //if (sockets.Length() > 1)
    //{
    //    // send message with true val
    //    sf::Packet packet;
    //    packet << 1;
    //    auto sendToOne = [&packet](std::shared_ptr<sf::TcpSocket> socket) {
    //        socket->send(packet);
    //    };
    //    sockets.for_each(sendToOne);
    //    std::cout << "Sent" << std::endl;
    //}
    // ****************************************
// Initialise
    srand(time(NULL));
    RenderWindow app(VideoMode(640, 480), "Server");
    app.setFramerateLimit(60);
    Texture t1, t2;
    t1.loadFromFile("images/background.png");
    t2.loadFromFile("images/car.png");
    t1.setSmooth(true);
    t2.setSmooth(true);
    Sprite sBackground(t1), sCar(t2);
    sBackground.scale(2, 2);
    sCar.setOrigin(22, 22);
    float R = 22;
    const int N = 5;
    const int WIDTH = 2880;
    const int HEIGHT = 3648;
    Car car[N];
    Color colors[5] = { Color::Red, Color::Green, Color::Magenta, Color::Blue, Color::White };

    // Starting positions
    for (int i = 0; i < N; i++)
    {
        car[i].x = 300 + i * 50;
        car[i].y = 1700 + i * 80;
        car[i].speed = 7 + ((float)i / 5);
    }
    float speed = 0, angle = 0;
    float maxSpeed = 12.0;
    float acc = 0.2, dec = 0.3;
    float turnSpeed = 0.08;
    int offsetX = 0, offsetY = 0;
    bool running = true;
    // GAME
    while (running)
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
            {
                app.close();
                running = false;
            }
        }
        // Step 3: Render
        app.clear(Color::White);
        // TODO: Stay within the limit of the map.
        for (int i = 0; i < N; i++)
        {
            if (car[i].x < 0 + sCar.getGlobalBounds().width / 2)
                car[i].x = 0 + sCar.getGlobalBounds().width / 2;
            if (car[i].x > sBackground.getGlobalBounds().width - sCar.getGlobalBounds().width / 2)
                car[i].x = sBackground.getGlobalBounds().width - sCar.getGlobalBounds().width / 2;

            if (car[i].y < 0 + sCar.getGlobalBounds().height / 2)
                car[i].y = 0 + sCar.getGlobalBounds().height / 2;
            if (car[i].y > sBackground.getGlobalBounds().height - sCar.getGlobalBounds().height / 2)
                car[i].y = sBackground.getGlobalBounds().height - sCar.getGlobalBounds().height / 2;
        }
        // TODO: Don't show white at bottom/right.
        float backPosX, backPosY;
        if (car[0].x > 320) offsetX = car[0].x - 320;
        if (car[0].y > 240) offsetY = car[0].y - 240;
        backPosX = offsetX;
        backPosY = offsetY;
        if (car[0].x >= sBackground.getGlobalBounds().width - 320)
        {
            backPosX = static_cast<float>(sBackground.getGlobalBounds().width - 640);
            offsetX = car[0].x - (320 + (car[0].x - (sBackground.getGlobalBounds().width - 320)));
        }
        if (car[0].y >= sBackground.getGlobalBounds().height - 240)
        {
            backPosY = static_cast<float>(sBackground.getGlobalBounds().height - 480);
            offsetY = car[0].y - (240 + (car[0].y - (sBackground.getGlobalBounds().height - 240)));
        }
        sBackground.setPosition(-backPosX, -backPosY);
        app.draw(sBackground);
        for (int i = 0; i < N; i++)
        {
            sCar.setPosition(car[i].x - offsetX, car[i].y - offsetY);
            sCar.setRotation(car[i].angle * 180 / 3.141593);
            sCar.setColor(colors[i]);
            app.draw(sCar);
        }
        app.display();



        
        
        for (int i = 0; i < N; i++) // send information of all other cars (excluding the cars message that's just been received)
        {
            Message msg = queue.pop();
            //std::cout << "Main read: \"" << msg.posX << " from " << msg.ID << "\"\n";

            sf::Packet packet;
            packet << msg;

            // receive information of cars
            car[msg.ID].x = msg.posX;
            car[msg.ID].y = msg.posY;
            car[msg.ID].speed = msg.speed;
            car[msg.ID].angle = msg.angle;
            acc = msg.acceleration;
            auto sendToOne = [&packet](std::shared_ptr<sf::TcpSocket> socket) {
                socket->send(packet);
            };
            sockets.for_each(sendToOne);
        }

        
    }

    accepterThread.join();
    return 0;
}
