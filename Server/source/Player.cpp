#include "Player.h"

Player::Player(std::unique_ptr<sf::TcpSocket>* socket, sf::Vector2f position, int id)
{
	m_position = position;
	m_name = "Default Name";
	m_id = id;
	m_socket = std::move(*socket);
	m_velocity = sf::Vector2f(0, 0);
	m_timeout = sf::seconds(0);
}


void Player::setPosition(sf::Vector2f position)
{
	m_position = position;
}

void Player::setPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}

sf::Vector2f Player::getPosition()
{
	return m_position;
}

void Player::setVelocity(sf::Vector2f velocity)
{
	m_velocity = velocity;
}

void Player::setVelocity(float x, float y)
{
	m_velocity.x = x;
	m_velocity.y = y;
}

sf::Vector2f Player::getVelocity()
{
	return m_velocity;
}

void Player::setName(const std::string name)
{
	m_name = name;
}

std::string Player::getName()
{
	return m_name;
}

int Player::getId()
{
	return m_id;
}

sf::TcpSocket* Player::getSocket()
{
	return m_socket.get();
}

void Player::setTimeout(sf::Time time)
{
	m_timeout = time;
}

sf::Time Player::getTimeout()
{
	return m_timeout;
}

void Player::update(float deltaTime)
{
	m_position.x += (m_velocity.x * deltaTime);
	m_position.y += (m_velocity.y * deltaTime);
	
}

