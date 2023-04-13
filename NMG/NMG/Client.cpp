#include "queue.h"
#include "Client.h"
#include "receiver.h"
#include "util.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <SFML/Graphics.hpp>
#include <cmath>

using namespace sf;

const int num = 8; //checkpoints
// TODO: use checkpoint to make sure we are on the track.
// Slow speed when not on the track.
int points[num][2] = { 300, 610,
    1270,430,
    1380,2380,
    1900,2460,
    1970,1700,
    2550,1680,
    2560,3150,
    500, 3300 };

struct Car
{
    float x, y, speed, angle; int n;
    Car() { speed = 2; angle = 0; n = 0; }
    void move()
    {
        x += sin(angle) * speed;
        y -= cos(angle) * speed;
    }
    void findTarget()
    {
        float tx = points[n][0];
        float ty = points[n][1];
        float beta = angle - atan2(tx - x, -ty + y);
        if (sin(beta) < 0) angle += 0.005 * speed; else angle -= 0.005 * speed;
        // Check if passed a checkpoint
        if ((x - tx) * (x - tx) + (y - ty) * (y - ty) < 25 * 25) n = (n + 1) % num; // TODO: simplify
    }
};

int Client::run()
{
    // set up network connection


    // TCP

    std::shared_ptr<sf::TcpSocket> TCPsocket = std::make_shared<sf::TcpSocket>();
    // auto connect = [&] { return socket->connect(sf::IpAddress::getLocalAddress(), PORT); };
    // net_run(connect, "connect");
    sf::Socket::Status status = TCPsocket->connect(sf::IpAddress::getLocalAddress(), PORT);
    if (status != sf::Socket::Done) {
        return 1;
    }
    std::cout << "TCP Connected\n";

    // UDP

    sf::UdpSocket UDPsocket;
   
    status = UDPsocket.bind(PORT);

    if (status != sf::Socket::Done) {
        return 1;
    }

    sf::Packet packet;
    std::cout << "UDP Connected\n";
    Queue<std::string> queue;
    // TODO launch a receiver thread to receive messages from the server. DONE
    std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(TCPsocket, queue);
    std::thread receiverThread(&Receiver::recv_loop, receiver);

    // UDP


    // ****************************************
   // Initialise
    srand(time(NULL));
    RenderWindow app(VideoMode(640, 480), "Car Racing Game!");
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


    while (1)
    {/*
        packet << "UDP broadcast";
        UDPsocket.send(packet, sf::IpAddress::Broadcast, PORT);
        sf::IpAddress sender;
        unsigned short port;
        UDPsocket.receive(packet, sender, port);
        std::string msg;
        packet >> msg;
        std::cout << msg << std::endl;*/
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }
        // Step 1: user input
        bool Up = 0, Right = 0, Down = 0, Left = 0;
        if (Keyboard::isKeyPressed(Keyboard::Up))    Up = 1;
        if (Keyboard::isKeyPressed(Keyboard::Right)) Right = 1;
        if (Keyboard::isKeyPressed(Keyboard::Down))  Down = 1;
        if (Keyboard::isKeyPressed(Keyboard::Left))  Left = 1;
        //
        // Step 2: update
        //car movement
        if (Up && speed < maxSpeed)
        {
            if (speed < 0)  speed += dec;
            else  speed += acc;
        }
        if (Down && speed > -maxSpeed)
        {
            if (speed > 0) speed -= dec;
            else  speed -= acc;
        }
        if (!Up && !Down)
        {
            if (speed - dec > 0) speed -= dec;
            else if (speed + dec < 0) speed += dec;
            else speed = 0;
        }
        if (Right && speed != 0)  angle += turnSpeed * speed / maxSpeed;
        if (Left && speed != 0)   angle -= turnSpeed * speed / maxSpeed;
        car[0].speed = speed;
        car[0].angle = angle;
        for (int i = 0; i < N; i++) car[i].move();
        for (int i = 1; i < N; i++) car[i].findTarget();
        //collision
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (i == j)
                {
                    break;
                }
                int dx = 0, dy = 0;
                while (dx * dx + dy * dy < 4 * R * R)
                {
                    car[i].x += dx / 10.0;
                    car[i].x += dy / 10.0;
                    car[j].x -= dx / 10.0;
                    car[j].y -= dy / 10.0;
                    dx = car[i].x - car[j].x;
                    dy = car[i].y - car[j].y;
                    if (!dx && !dy) break;
                }
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
    }
    receiverThread.join();
    return 0;
}