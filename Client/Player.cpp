#include "Player.h"


Player::Player() : m_health(100), m_id(-1)
{
	if (!pTexture.loadFromFile("Media/player.png"))
		std::cout << "Error loading player.png" << std::endl;
	this->setTexture(pTexture);
}


Player::~Player()
{
}
