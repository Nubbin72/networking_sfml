#pragma once
#include <SFML/Network.hpp>
#include "Player.h"
#include "Enemy.h"
#include <iostream>
#include <memory>
class Network
{
public:
	Network(sf::IpAddress& ip, unsigned short& port);
	void disconnect(Player* p);

	void send(Player* p);
	void sendPosition(Player* p);
	void receive(std::vector<std::unique_ptr<Enemy>>& enemies, Player* p); //Add more later
	void sendMessage(Player* p, std::string& text);
	void sendMyName(Player* p);
	void getPlayerList(Player* p);

	std::string getMessage(){ return m_textMessage; }

private:
	sf::TcpSocket connection;
	bool m_connected;

	std::string m_textMessage;
	int packetCounter = 0;
	sf::Clock packetClock;
};

