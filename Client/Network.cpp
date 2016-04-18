#include "Network.h"


Network::Network(sf::IpAddress& ip, unsigned short& port)  : m_connected(false)
{
	if (connection.connect(ip, port, sf::seconds(5)) != sf::Socket::Done)
	{
		std::cout << "Error connecting to server" << std::endl;
	}
	else
	{
		connection.setBlocking(false);
		std::cout << "Connected" << std::endl;
	}
}


void Network::disconnect(Player* p) //Disconnect the player from the server
{
	sf::Packet temp;
	temp << 1;
	temp << p->getID();

	if (connection.send(temp) != sf::Socket::Done)
	{
		std::cout << "Error sending disconnect command to server" << std::endl;
	}
	else
	{
		std::cout << "Disconnected" << std::endl;
	}
}

void Network::send(Player* p)
{
	sf::Packet temp;
	temp << 3;
	temp << p->getID();
	temp << p->getDirection().x;
	temp << p->getDirection().y;

	if (connection.send(temp) != sf::Socket::Done)
	{
		std::cout << "Error sending data to server" << std::endl;
	}


}

void Network::sendPosition(Player* p)
{
	if(m_connected)
	{
		sf::Packet temp;
		temp << 4;
		temp << p->getID();
		temp << p->getPosition().x;
		temp << p->getPosition().y;

		if (connection.send(temp) != sf::Socket::Done)
		{
			std::cout << "Error sending data to server" << std::endl;
		}
	}

}


void Network::sendMessage(Player* p, std::string& text)
{
	sf::Packet temp;
	temp << 5;
	temp << p->getID();
	temp << text;

	if (text.length() > 2)
		if (connection.send(temp) != sf::Socket::Done)
		{
			std::cout << "Error sending text message to server" << std::endl;
		}
}


void Network::sendMyName(Player* p)
{
	sf::Packet temp;
	temp << 6;
	temp << p->getID();
	temp << p->getName();

	if (connection.send(temp) != sf::Socket::Done)
	{
		std::cout << "Error sending my name" << std::endl;
	}

}

void Network::getPlayerList(Player* p)
{
	sf::Packet temp;
	temp << 7;
	temp << p->getID();

	if (connection.send(temp) != sf::Socket::Done)
	{
		std::cout << "Error sending getPlayerList to server" << std::endl;
	}
}



void Network::receive(std::vector<std::unique_ptr<Enemy>>& enemies, Player* p)
{

	sf::Packet receivePacket;
	int type, id;
	if (connection.receive(receivePacket) == sf::Socket::Done)
	{
		receivePacket >> type;
		receivePacket >> id;

		if (type == 0) // you connected to server, get your ID
		{
			if (p->getID() == -1)
			{
				p->setID(id);
				std::cout << "I connected to server, my ID: " << p->getID() << std::endl;
				this->sendMyName(p);
				sf::sleep(sf::milliseconds(50));
				this->getPlayerList(p);
			}
			m_connected = true;
		}
		else if (type == 1) // disconnected
		{
			for (unsigned int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i]->getID() == id)
				{
					m_textMessage = "Player " + enemies[i]->getName() + " disconnected.";
					std::cout << "Enemy: " << enemies[i]->getID() << " deleted " << std::endl;
					enemies.erase(enemies.begin() + i);
				}
			}
		}
		else if (type == 2)
		{
			std::cout << "Server is full" << std::endl;
		}

		else if (type == 3) // get move direction of enemys
		{
			for (unsigned int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i]->getID() == id)
				{
					sf::Vector2f dir;
					receivePacket >> dir.x;
					receivePacket >> dir.y;

					enemies[i]->setDirection(dir);
					break;
				}

			}
		}
		else if (type == 4) //Set player position to the recived position
		{

			for (unsigned int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i]->getID() == id)
				{
					sf::Vector2f pos;
					receivePacket >> pos.x;
					receivePacket >> pos.y;

					enemies[i]->setPosition(pos);
					break;
				}

			}
		}
		else if (type == 5) // chat message received
		{
			std::string receivedMessage;
			receivePacket >> receivedMessage;

			if (!receivedMessage.empty())
			{
				std::string senderName;
				for (unsigned int k = 0; k < enemies.size(); k++)
				{
					if (enemies[k]->getID() == id)
						senderName = enemies[k]->getName();
				}
				if (id == p->getID())
					senderName = p->getName();
				std::string testMessage(receivedMessage);
				std::string newString = testMessage.substr(0, testMessage.length() - 1);
				m_textMessage = senderName + ":" + newString;
			}


		}
		//Number 6 is reserved by the server for saving the name

		else if (type == 7) //Create new players
		{
			int playerNumber;
			std::vector<std::string> playersName;
			std::vector<int> playersId;

			receivePacket >> playerNumber;
			std::cout << "Num of players on server: " << playerNumber << std::endl;

			for (int i = 0; i < playerNumber; ++i)
			{
				std::string tempName;
				int tempId;
				receivePacket >> tempId;
				receivePacket >> tempName;
				playersName.push_back(tempName);
				playersId.push_back(tempId);

			}


			for (unsigned int i = 0; i < playersId.size(); ++i) //loop through id-s we got
			{
				bool haveThatEnemy = false;
				for (unsigned int v = 0; v < enemies.size(); v++) //check if we already have enemy with that id
				{
					if (enemies[v]->getID() == playersId[i])
					{
						haveThatEnemy = true;
					}

				}
				if (playersId[i] != p->getID() && !haveThatEnemy) //if it is not our id and if we dont have that enemy, create a new enemy with that id
				{
					enemies.push_back(std::make_unique<Enemy>(playersId[i], sf::Vector2f(100, 100), playersName[i]));
					m_textMessage = "New player connected: " + playersName[i];
					std::cout << "Created a new enemy with id: " << playersId[i] << std::endl;

				}
			}

			playersName.clear();
			playersId.clear();

		}


	}

}
