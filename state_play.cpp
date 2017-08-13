#include <SFML/Graphics.hpp>

#include "state_play.hpp"
#include "state.hpp"
#include "state_edit.hpp"

#include <iostream>
#include <string>
#include <fstream>

void StatePlay::draw(Game* game, const float dt)
{
	game->window.clear(sf::Color::Black);

	game->window.setView(worldView);
	this->level.draw(game->window, dt);
	this->level.drawPlayer(game->window, dt);
	// TODO background again. dunno if this will happen.
	// this->game->window.draw(this->game->background);

	return;
}

void StatePlay::update(Game* game, const float dt)
{
	level.time += dt;
	// TODO here do if level.level finished, load the next level into level.
	if (level.passedLevel())
	{
		//std::cout << "level" << levelNum << "passed!\n";
		// update the completed level list
		std::vector<char> levelsPassed;

		std::ifstream nf("game.dat");
		std::copy(std::istream_iterator<char>(nf), std::istream_iterator<char>(), std::back_inserter(levelsPassed));

		//std::cout << levelsPassed[0];
		levelsPassed[levelNum - 1] = 1;
		nf.close();
		
		std::ofstream of("game.dat");
		std::copy(levelsPassed.begin(), levelsPassed.end(), std::ostream_iterator<char>(of));
		// yay
		of.close();
		std::string filename = "levels/level" + std::to_string(levelNum + 1) + ".level";
		std::ifstream nextFile(filename.c_str());
		if (nextFile.good())
		{
			level = Level("levels/level" + std::to_string(++levelNum));

		}
		else
		{
			game->popState();
			game->getState()->init(game);
		}
		nextFile.close();
			
			
	}
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
		// TODO include resizing code here/ disable
		case sf::Event::KeyPressed:
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				game->popState();
				game->getState()->init(game);
			}
				
			if (event.key.code == sf::Keyboard::Return)
			{
				game->changeState(new StateEdit(level));
			}
			if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
				level.input(Direction::UP);
			if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
				level.input(Direction::DOWN);
			if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
				level.input(Direction::LEFT);
			if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
				level.input(Direction::RIGHT);
			if (event.key.code == sf::Keyboard::Z)
				level.undo();
			if (event.key.code == sf::Keyboard::R)
				level.reload();
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
	level.settleEntities();
	level.updateTrees();
	level.reset();
	level.clearHistory();
}

StatePlay::StatePlay()
{
	level = Level("levels/level1");
}

StatePlay::StatePlay(Level level)
{
	this->level = level;
	this->levelNum = 1;
}

StatePlay::StatePlay(int levelNum)
{
	level = Level("levels/level" + std::to_string(levelNum));
	this->levelNum = levelNum;
}

