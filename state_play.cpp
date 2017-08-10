#include <SFML/Graphics.hpp>

#include "state_play.hpp"
#include "state.hpp"
#include "state_edit.hpp"

void StatePlay::draw(Game* game, const float dt)
{
	game->window.clear(sf::Color::Black);

	game->window.setView(worldView);
	this->level.draw(game->window, dt);
	this->level.drawPlayer(game->window, dt);
	//background again. dunno if this will happen.
	// this->game->window.draw(this->game->background);

	return;
}

void StatePlay::update(Game* game, const float dt)
{
	return;
}

void StatePlay::handleInput(Game* game)
{
	sf::Event event;

	sf::Vector2f position = game->window.mapPixelToCoords(sf::Mouse::getPosition(game->window), this->worldView);

	while (game->window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
		{
			game->window.close();
			break;
		}
		// include resizing code here
		case sf::Event::KeyPressed:
		{
			if (event.key.code == sf::Keyboard::Escape)
				game->popState();
			if (event.key.code == sf::Keyboard::Return)
			{
				game->changeState(new StateEdit(level));
			}
			break;
		}
		default: break;
		}
	}
}

void StatePlay::init(Game* game)
{
	sf::Vector2f pos = sf::Vector2f(game->window.getSize());
	this->worldView.setSize(pos);
	pos *= 0.5f;
	this->worldView.setCenter(pos);

	level.selectTileByPos(sf::Vector2f(-1, -1));
}

StatePlay::StatePlay()
{
	level = Level("test", 8, 8);
}

StatePlay::StatePlay(Level level)
{
	this->level = level;
}

