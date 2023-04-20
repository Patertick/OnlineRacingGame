#ifndef CAR_H
#define CAR_H
#include <cmath>
#include <algorithm>
#include <functional>
#include "Message.h"
#include "Checkpoints.h"
#include <SFML/Network.hpp>

static const int num{ 8 };//checkpoints
//// TODO: use checkpoint to make sure we are on the track.
//// Slow speed when not on the track.
//

static int points[num][2] = { 300, 610,
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
    
    int nextCheckpoint{ 1 };
    int lap{ 0 }; // laps, increment when checkpoints have all been passed

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
    void findCheckpoint(std::function<bool(int, int)> fX, std::function<bool(int, int)> fY)
    {
        if (fX(x, points[nextCheckpoint][0])) // have we passed the next checkpoint?
        {
            if (fY(y, points[nextCheckpoint][1]))
            {
                if (nextCheckpoint == 0)
                {
                    lap++; // increment lap
                    std::cout << "Lapped course" << std::endl;
                }
                std::cout << "Cars X: " << x << std::endl;
                std::cout << "Cars Y: " << y << std::endl;
                std::cout << "Checkpoint X: " << points[nextCheckpoint][0] << std::endl;
                std::cout << "Checkpoint Y: " << points[nextCheckpoint][1] << std::endl;
                nextCheckpoint++; // increment currentCheckpoint
                if (nextCheckpoint >= num) // check if current checkpoint is last one in points
                {
                    nextCheckpoint = 0; // reset current checkpoint
                }

                std::cout << nextCheckpoint << " is our current checkpoint" << std::endl;
            }
        }

    }
    sf::Packet getPacket(float pAcceleration, int pID)
    {
        Message msg;
        msg.posX = x;
        msg.posY = y;
        msg.speed = speed;
        msg.angle = angle;
        msg.acceleration = pAcceleration;
        msg.ID = pID;
        sf::Packet packet;
        packet << msg;
        return packet;
    }
};
#endif