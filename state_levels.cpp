#include <SFML/Graphics.hpp>

#include "state_levels.hpp"
#include "state.hpp"
#include "state_play.hpp"

#include <iostream>
#include <string>
#include <sys/stat.h>

void LevelButton::draw(sf::RenderWindow& window, float dt)
{
	this->sprthlpr.update(dt);

	this->sprite.setTextureRect(this->sprthlpr.getBounds());

	window.draw(this->sprite);
}

void LevelButton::update()
{
}

LevelButton::LevelButton(sf::Texture& texture, SpriteInfo sprinf, SpriteInfo sprinf2, int level)
{
	this->sprite.setTexture(texture);
	this->sprthlpr.setFrameSize(sf::IntRect(0, 0, 64, 64));

	this->sprthlpr.initVersion(sprinf);
	this->sprthlpr.initVersion(sprinf2);

	this->level = level;
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

			if (y * 12 + x == level)
				this->levelButtons[y * 12 + x].sprthlpr.changeVersion(1);
			else
				this->levelButtons[y * 12 + x].sprthlpr.changeVersion(0);

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
	}
	case 2:
	{
		// Clear
		// TODO the save clearing logic
	}
	case 3:
	{
		// Select level
		struct stat buffer;
		std::string filename = "level" + std::to_string(level + 1) + ".level";
		std::cout << filename;
		if (stat(filename.c_str(), &buffer) == 0)
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

	// load the level statuses from a file
	// 

	// TODO this will later involve a fstream, but not now
	for (int index = 0; index < 60; ++index)
	{
		levelButtons.push_back(
			LevelButton(buttonTex, SpriteInfo(sf::Vector2i(0, 0)), SpriteInfo(sf::Vector2i(64, 0)), index + 1)); // can probably remove the last argument in this constructor
	}
}

StateLevels::StateLevels()
{
	font.loadFromFile("verdanab.ttf");


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
	buttonTex.loadFromFile("media/levelbuttons.png");

	updateMenu();

}