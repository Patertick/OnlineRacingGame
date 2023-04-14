#include<iostream>
#include <SFML/Network.hpp>

struct Message {
	float posX;
	float posY;
	float speed;
	float angle;
	float acceleration;
	int ID;

	friend sf::Packet& operator << (sf::Packet& pOut, const Message& pMsg)
	{
		pOut << pMsg.posX << pMsg.posY << pMsg.speed << pMsg.angle << pMsg.angle << pMsg.acceleration << pMsg.ID;
		return pOut;
	}
	friend sf::Packet& operator >> (sf::Packet& pIn, Message& pMsg)
	{
		pIn >> pMsg.posX >> pMsg.posY >> pMsg.speed >> pMsg.angle >> pMsg.angle >> pMsg.acceleration >> pMsg.ID;
		return pIn;
	}
};
