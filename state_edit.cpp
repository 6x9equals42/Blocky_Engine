#include <SFML/Graphics.hpp>

#include "state_edit.hpp"
#include "state.hpp"

#include <iostream>

void StateEdit::draw(Game* game, const float dt)
{
	game->window.clear(sf::Color::Black);

	game->window.setView(worldView);
	this->level.draw(game->window, dt);
	//background again. dunno if this will happen.
	// this->game->window.draw(this->game->background);

	return;
}

void StateEdit::update(Game* game, const float dt)
{
	return;
}

void StateEdit::handleInput(Game* game)
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
			if (event.key.control && event.key.code == sf::Keyboard::S)
			{
				this->level.save("test");
			}
			if (event.key.code == sf::Keyboard::Num1)
			{
				this->level.changeTile(TileType::INACCESSIBLE);
			}
			if (event.key.code == sf::Keyboard::Num2)
			{
				this->level.changeTile(TileType::FLAT);
			}
			if (event.key.code == sf::Keyboard::Num3)
			{
				this->level.changeTile(TileType::WATERWAY);
			}
			if (event.key.code == sf::Keyboard::Num4)
			{
				this->level.changeTile(TileType::WATERSOURCE);
			}
			if (event.key.code == sf::Keyboard::Num5)
			{
				this->level.changeTile(TileType::DRAIN);
			}
			if (event.key.code == sf::Keyboard::Num6)
			{
				this->level.changeTile(TileType::BRIDGE);
			}
			if (event.key.code == sf::Keyboard::Num7)
			{
				this->level.changeTile(TileType::SAPLING);
			}
			if (event.key.code == sf::Keyboard::Num8)
			{
				this->level.changeTile(TileType::COAL);
			}
			//if (event.key.code == sf::Keyboard::Return)
			//	menuSelect();
			break;
		}
		case sf::Event::MouseButtonPressed:
		{
			std::cout << position.x << " " << position.y << "\n";
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				level.selectTileByPos(position);
			}
			break;
		}
		default: break;
		}
	}
}

void StateEdit::init(Game* game)
{
	sf::Vector2f pos = sf::Vector2f(game->window.getSize());
	this->worldView.setSize(pos);
	pos *= 0.5f;
	this->worldView.setCenter(pos);
}

StateEdit::StateEdit()
{
	
	//level = Level();
	//level.createBlankLevel(); // for creating new levels..

	level = Level("test", 8, 8);
}
