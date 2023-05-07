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

static int points[num][2] = { 450, 610,
    1200,530,
    1480,2400,
    1750,2460,
    2100,1710,
    2350,1700,
    2360,3050,
    451, 3170 };

struct Car
{
    float x, y, speed, angle; int n, place;
    bool finishedRace{ false };
    std::function<bool(int, int)> currentFunX, currentFunY;
    int nextCheckpoint{ 0 };
    int lap{ 0 }; // laps, increment when checkpoints have all been passed

    Car() { speed = 2; angle = 0; n = 0; }
    void move()
    {
        if (!finishedRace)
        {
            x += sin(angle) * speed;
            y -= cos(angle) * speed;

        }
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
        currentFunX = fX;
        currentFunY = fY;
        if (fX(x, points[nextCheckpoint][0])) // have we passed the next checkpoint?
        {
            if (fY(y, points[nextCheckpoint][1]))
            {
                if (nextCheckpoint == 0)
                {
                    lap++; // increment lap
                    if (lap > 3)
                    {
                        finishedRace = true;
                    }
                    //std::cout << "Lapped course" << std::endl;
                }
                /*std::cout << "Cars X: " << x << std::endl;
                std::cout << "Cars Y: " << y << std::endl;
                std::cout << "Checkpoint X: " << points[nextCheckpoint][0] << std::endl;
                std::cout << "Checkpoint Y: " << points[nextCheckpoint][1] << std::endl;*/
                nextCheckpoint++; // increment currentCheckpoint
                if (nextCheckpoint >= num) // check if current checkpoint is last one in points
                {
                    nextCheckpoint = 0; // reset current checkpoint
                }

                std::cout << nextCheckpoint << " is our current checkpoint" << std::endl;
            }
        }

    }
    bool operator ==(Car& other)
    {
        if (x == other.x && y == other.y)
        {
            return true;
        }
        return false;
    }
    bool operator >(Car& other)
    {
        // check lap first
        if (lap > other.lap)
        {
            return true;
        }
        else if (other.lap > lap)
        {
            return false;
        }

        // then check checkpoints
        if (nextCheckpoint > other.nextCheckpoint)
        {
            return true;
        }
        else if (other.nextCheckpoint > nextCheckpoint)
        {
            return false;
        }
        // finally check actual positions
        switch (nextCheckpoint)
        {
        case 0:
        case 4:
            if (y < other.y)
            {
                return true;
            }
            else
            {
                return false;
            }
            break;
        case 2:
        case 6:
            if (y > other.y)
            {
                return true;
            }
            else
            {
                return false;
            }
            break;
        case 1:
        case 3:
        case 5:
            if (x > other.x)
            {
                return true;
            }
            else
            {
                return false;
            }
            break;
        case 7:
            if (x < other.x)
            {
                return true;
            }
            else
            {
                return false;
            }
            break;

        }

        // either cars are in exact same position (which is impossible due to collision) or other car is further
        return false; // thus we return false
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