#include "Game.h"

const sf::Time Game::timePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
{
	m_positionTimer = sf::Time::Zero;
	port = 45000;
	ipAdress = "localhost";

	network = std::make_unique<Network>(ipAdress, port);

	pSprite.setName("Nikola");

	mWindow.create(sf::VideoMode(840, 680), "Game");
	socket.setBlocking(false);
	mWindow.setFramerateLimit(60);

	dir[0] = 0;
	dir[1] = 0;
	dir[2] = 0;
	dir[3] = 0;

	srand(time(0));


}

Game::~Game()
{
	network->disconnect(&pSprite);
}
void Game::run()
{
    //TODO fix timestep
	while (mWindow.isOpen())
	{
		procesEvents();
		update();
		render();
	}
}


void Game::procesEvents()
{
    //TODO organize events
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			mWindow.close();
			break;
		case sf::Event::KeyPressed:
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				mWindow.close();
				break;
			}
			break;
		case sf::Event::KeyReleased:
			switch (event.key.code)
			{
			}
			break;

		}
	}

    //TODO Organize input, Speed times delta time
	if (mWindow.hasFocus())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !movingDiagonal)
		{
			pSprite.move(0, -2);
			pSprite.setDirection(sf::Vector2f(0, -2));
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !movingDiagonal)
		{
			pSprite.move(-2, 0);
			pSprite.setDirection(sf::Vector2f(-2, 0));
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !movingDiagonal)
		{
			pSprite.move(0, 2);
			pSprite.setDirection(sf::Vector2f(0, 2));
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !movingDiagonal)
		{
			pSprite.move(2, 0);
			pSprite.setDirection(sf::Vector2f(2, 0));
		}


		//Up Right
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			movingDiagonal = true;
			pSprite.move(1.4, -1.4);
			pSprite.setDirection(sf::Vector2f(1.4, -1.4));
		}
		//Up Left
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			movingDiagonal = true;
			pSprite.move(-1.4, -1.4);
			pSprite.setDirection(sf::Vector2f(-1.4, -1.4));
		}
		//Down Right
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			movingDiagonal = true;
			pSprite.move(1.4, 1.4);
			pSprite.setDirection(sf::Vector2f(1.4, 1.4));
		}
		//Down Left
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			movingDiagonal = true;
			pSprite.move(-1.4, 1.4);
			pSprite.setDirection(sf::Vector2f(-1.4, 1.4));
		}
		else
			movingDiagonal = false;

		//IF no button is pressed
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			pSprite.setDirection(sf::Vector2f(0, 0));

	}


}


void Game::update()
{
	//Send direction
	if (lastDirSent != pSprite.getDirection())
	{
		network->send(&pSprite); // send move data
		lastDirSent = pSprite.getDirection();
	}
	//Send position 1 time every 1 second
	m_positionTimer += m_clock.restart();
	if (m_positionTimer >= sf::seconds(1))
	{
		network->sendPosition(&pSprite);
		m_positionTimer = sf::Time::Zero;
	}

	//Recive data from network
	network->receive(enemies, &pSprite);


	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->move(enemies[i]->getDirection());
	}

}


void Game::render()
{
	mWindow.clear(sf::Color::Cyan);
	for (unsigned int i = 0; i < enemies.size(); i++)
		mWindow.draw(*enemies[i]);
	mWindow.draw(pSprite);
	mWindow.display();
}
