#pragma once
#include<iostream>
#include <SFML/Network.hpp>

struct Message {
	float posX{ 0 };
	float posY{ 0 };
	float speed{ 0 };
	float angle{ 0 };
	float acceleration{ 0 };
	int ID{ -1 };

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
