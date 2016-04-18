#include "Server.h"


Server::Server()
{
	m_maxPlayerNumber = 5 ;
	m_currentID = 0;
	m_playerNumber = 0;

	m_isRunning = true;
	m_listener.listen(45000);
	m_selector.add(m_listener);
	std::cout << "Server is started. Waiting for clients" << std::endl;
}

void Server::run()
{
	while (m_isRunning)
	{
		if (m_selector.wait())
		{
			if (m_selector.isReady(m_listener)) // if server is ready to receive new connections
			{
				std::unique_ptr<sf::TcpSocket> tempSocket = std::make_unique<sf::TcpSocket>();

				if (m_listener.accept(*tempSocket) == sf::Socket::Done)
				{
					if (m_playerNumber < m_maxPlayerNumber) //if server is not full
					{
						m_playerList.emplace_back(Player(&tempSocket, sf::Vector2f(0,0), m_currentID));
						m_selector.add(*m_playerList.back().getSocket());
						m_playerNumber++;

						sf::Packet outPacket;
						outPacket << 0;
						outPacket << m_currentID;

						if (m_playerList.back().getSocket()->send(outPacket) != sf::Socket::Done) //Send client id
							std::cout << "error sending player index" << std::endl;
						m_currentID++;
					}
					else //if server is full
					{
						sf::Packet outPacket;
						outPacket << 2;
						outPacket << 0;

						if (tempSocket->send(outPacket) != sf::Socket::Done)
							std::cout << "Error sending server is full message" << std::endl;

						std::cout << "User tried to connect but the server was full" << std::endl;
					}
				}


			}
			else
			{
				//Receive data
				for (unsigned int i = 0; i < m_playerList.size(); i++)
				{
					if (m_selector.isReady(*m_playerList[i].getSocket()))
					{
						sf::Packet received;
						if (m_playerList[i].getSocket()->receive(received) == sf::Socket::Done)
						{
							m_playerList[i].setTimeout(m_clock.getElapsedTime());

							int num, id;
							received >> num;
							received >> id;


							// 0 is sent when player establishes a connection to the server

							if (num == 1) // player disconnected, send message to all players to delete his character and delete him from servers players list
							{

								sendPacket(received, i);

								for (auto& itr : m_playerList)
								{
									if (itr.getId() == id)
									{

										std::cout << std::endl << "Client disconnected!" << std::endl;
										std::cout << "	ID: " << itr.getId() << " Name: " << itr.getName() << std::endl;
									}
								}

								std::cout << "Number of players: " << m_playerNumber << std::endl;
								m_selector.remove(*m_playerList[i].getSocket());
								m_playerList.erase(m_playerList.begin() + i);
								m_playerNumber--;
								break;
							}
							//Number 2 means the server is full
							//it is sent only to the player who cannot connect

							else if (num == 3) //Moving direction
							{
								if (id != -1)
								{
									sendPacket(received, i);
								}

							}
							else if (num == 4) //Position
							{
								if (id != -1)
								{
									sendPacket(received, i);
								}
							}
							else if (num == 5) //Send text message
							{
								for (unsigned int k = 0; k < m_playerList.size(); k++)
								{
									sendPacket(received);
									std::string messageSent;
									std::cout << "Sent: " << (received >> messageSent) << std::endl;

								}
							}
							else if (num == 6) //Save player name
							{
								if (m_playerList[i].getId() == id)
								{
									std::string nameHolder;
									received >> nameHolder;
									m_playerList[i].setName(nameHolder);
									std::cout << std::endl << std::endl << "New client added." << std::endl;
									std::cout << "	ID: " << id << " Name: " << nameHolder << std::endl;
									std::cout << "Number of players: " << m_playerNumber << std::endl;
								}
							}
							else if (num == 7) //send client list with id and names  When player recive this it goes through the list and compares it with its list, if he finds a number that he doesn't have he creates a enemy with that id
							{
								sf::Packet namePacket;
								namePacket << 7;
								namePacket << 0;
								namePacket << m_playerNumber;

								for (unsigned int j = 0; j < m_playerList.size(); ++j)
								{
									namePacket << m_playerList[j].getId();
									namePacket << m_playerList[j].getName();
								}

								sendPacket(namePacket);
							}

						}



						//If some player time-out-ed alert other players
						float tempTime = m_clock.getElapsedTime().asSeconds() - m_playerList[i].getTimeout().asSeconds();
						if (tempTime >= 5)
						{
							sf::Packet timeOutPacket;
							timeOutPacket << 2;
							timeOutPacket << m_playerList[i].getId();

							std::cout << "Player: " << m_playerList[i].getId() << " timeouted" << std::endl;
							sendPacket(received, i);

							m_selector.remove(*m_playerList[i].getSocket());
							m_playerList.erase(m_playerList.begin() + i);
							m_playerNumber--;
							break;
						}
					} // end of player socket is ready
				}
			}
		}

	}
}



void Server::sendPacket(sf::Packet & packet, unsigned int skip)
{
	for (unsigned int i = 0; i < m_playerList.size(); ++i)
	{
		if (skip == i)
			continue;
		if (m_playerList[i].getSocket()->send(packet) != sf::Socket::Done)
		{
			std::cout << "Error sending packet in sendPacket func" << std::endl;
		}
	}
}
