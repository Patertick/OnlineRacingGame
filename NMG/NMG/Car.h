#ifndef CAR_H
#define CAR_H
#include <cmath>
#include "Message.h"
#include "Checkpoints.h"
#include <SFML/Network.hpp>




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