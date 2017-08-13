#include <SFML/Graphics.hpp>

#include "state_levels.hpp"
#include "state.hpp"
#include "state_play.hpp"

#include <fstream>
#include <iostream>
#include <string>

void LevelButton::draw(sf::RenderWindow& window, float dt)
{
	this->sprthlpr.changeVersion(buttonVersion);

	this->sprthlpr.update(dt);

	this->sprite.setTextureRect(this->sprthlpr.getBounds());

	window.draw(this->sprite);
}

void LevelButton::update()
{
}

LevelButton::LevelButton(sf::Texture& texture, const std::vector<SpriteInfo>& sprinfs, int level)
{
	this->sprite.setTexture(texture);
	this->sprthlpr.setFrameSize(sf::IntRect(0, 0, 64, 64));

	for (auto sprinf : sprinfs)
	{
		this->sprthlpr.initVersion(sprinf);
	}

	this->level = level;
	this->buttonVersion = 0;
}

LevelButton::LevelButton()
{}

// Now for StateLevels:

void StateLevels::draw(Game* game, const float dt)
{
	// set the view so that resizing works.
	game->window.setView(this->view);

	game->window.clear(sf::Color::Black);


	// Loop through all the levelButtons, do them 12 per row for now.
	for (int y = 0; y < 5; ++y)
	{
		for (int x = 0; x < 12; ++x)
		{
			sf::Vector2f pos;
			pos.x = x * 64 + 128;
			pos.y = y * 64 + 128;
			this->levelButtons[y*12 + x].sprite.setPosition(pos);

			if (levelsPassed[y * 12 + x] == 1)
			{
				if (y * 12 + x == level)
					this->levelButtons[y * 12 + x].buttonVersion = 1;
				else
					this->levelButtons[y * 12 + x].buttonVersion = 0;
			}
			else
			{
				if (y * 12 + x == level)
					this->levelButtons[y * 12 + x].buttonVersion = 3;
				else
					this->levelButtons[y * 12 + x].buttonVersion = 2;
			}
			

			// then draw it

			this->levelButtons[y * 12 + x].draw(game->window, dt);

			numbers.setString(std::to_string(y * 12 + x + 1));
			numbers.setPosition(pos.x + 16, pos.y + 15);

			game->window.draw(numbers);

		}
	}

	game->window.draw(menuBack);
	game->window.draw(menuClear);

	return;
}

void StateLevels::update(Game* game, const float dt)
{}

void StateLevels::handleInput(Game* game)
{
	sf::Event event;

	while (game->window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
		{
			game->window.close();
			break;
		}
		// TODO include resizing code here
		case sf::Event::KeyPressed:
		{
			if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)
				menuDown();
			if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up)
				menuUp();
			if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left)
				menuLeft();
			if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right)
				menuRight();
			if (event.key.code == sf::Keyboard::Escape)
				game->popState();
			if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space)
				menuSelect(game);
			break;
		}
		default: break;
		}
	}
}

void StateLevels::menuUp()
{
	if (menu < 3)
	{
		menu = 3;
		level = 48;
	}
	else if (level > 11)
		level -= 12;
	updateMenu();
}

void StateLevels::menuDown()
{
	if (menu < 3)
	{
	}// do nothing
	else if (level >= 48 && menu == 3)
		menu = 1;
	else if (level < 48)
		level += 12;
	updateMenu();
}

void StateLevels::menuLeft()
{
	if (menu == 2)
		menu = 1;
	else if (menu == 1)
		menu = 1;
	else if (level % 12 != 0)
		level -= 1;
	updateMenu();
}

void StateLevels::menuRight()
{
	if (menu == 1)
		menu = 2;
	else if (menu == 2)
		menu = 2;
	if (level % 12 != 11)
		level += 1;
	updateMenu();
}

void StateLevels::updateMenu()
{
	menuBack.setFillColor(sf::Color::White);
	menuClear.setFillColor(sf::Color::White);

	switch (menu)
	{
	case 1:
	{
		menuBack.setFillColor(sf::Color::Yellow);
		level = -1;
		break;
	}
	case 2:
	{
		menuClear.setFillColor(sf::Color::Yellow);
		level = -1;
		break;
	}
	case 3:
	{
	}
	default:
		break;
	}
}

void StateLevels::menuSelect(Game* game)
{
	switch (this->menu)
	{
	case 1:
	{
		// back
		game->popState();
		break;
	}
	case 2:
	{
		// Clear
		// TODO the save clearing logic
		levelsPassed.clear();
		levelsPassed.assign(60, 0);

		// then save the data to file.
		std::ofstream f("game.dat");
		std::copy(this->levelsPassed.begin(), this->levelsPassed.end(), std::ostream_iterator<char>(f));


		f.close();
		break;
	}
	case 3:
	{
		// Select level
		
		std::string filename = "levels/level" + std::to_string(level + 1) + ".level";
		std::ifstream f(filename.c_str());
		if (f.good())
			game->pushState(new StatePlay(level + 1));
		break;
	}
	default:
		break;
	}
}

void StateLevels::init(Game* game)
{
	sf::Vector2f pos = sf::Vector2f(game->window.getSize());
	this->view.setSize(pos);
	pos *= 0.5f;
	this->view.setCenter(pos);

	levelButtons.clear();
	levelsPassed.clear();
	for (int index = 0; index < 60; ++index)
	{
		levelButtons.push_back(
			LevelButton(buttonTex, 
			{ SpriteInfo(sf::Vector2i(0, 0)),
				SpriteInfo(sf::Vector2i(64, 0)),
				SpriteInfo(sf::Vector2i(0, 64)),
				SpriteInfo(sf::Vector2i(64, 64)) },
				index + 1)); // can probably remove the last argument in this constructor
	}

	// load the level statuses
	std::ifstream f("game.dat");
	std::copy(std::istream_iterator<char>(f), std::istream_iterator<char>(), std::back_inserter(levelsPassed));

	f.close();
}

StateLevels::StateLevels()
{
	font.loadFromFile("resources/verdanab.ttf");


	// track what menu item is highlighted
	menu = 3;
	level = 0;

	// Create the menu text parts
	menuBack.setFont(font);
	menuBack.setCharacterSize(60);
	menuBack.setString("BACK");
	menuBack.setPosition(280.f, 576.f);

	menuClear.setFont(font);
	menuClear.setCharacterSize(60);
	menuClear.setString("CLEAR");
	menuClear.setPosition(576.f, 576.f);

	numbers.setFont(font);
	numbers.setCharacterSize(20);


	// and create the texture
	buttonTex.loadFromFile("resources/levelbuttons.png");

	updateMenu();

}