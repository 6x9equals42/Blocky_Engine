#include <SFML/Graphics.hpp>

#include "state_menu.hpp"
#include "state.hpp"
#include "state_edit.hpp"
#include "state_play.hpp"
#include "state_levels.hpp"

//#include <iostream>

void StateMenu::draw(Game* game, const float dt)
{
	// set the view so that resizing works.
	game->window.setView(this->view);

	game->window.clear(sf::Color::Black);


	// TODO for the background of the title screen once I have a sf::sprite background
	// this->game->window.draw(this->game->background);

	game->window.draw(menuPlay);
	game->window.draw(menuEdit);
	game->window.draw(menuExit);

	return;
}

void StateMenu::update(Game* game, const float dt)
{}

void StateMenu::handleInput(Game* game)
{
	sf::Event event;

	sf::Vector2f position = game->window.mapPixelToCoords(sf::Mouse::getPosition(game->window), this->view);

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
			if (event.key.code == sf::Keyboard::Escape)
				game->window.close();
			if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space)
				menuSelect(game);
			break;
		}
		case sf::Event::MouseMoved:
		{
			if (checkMouseMenu(position))
			{
				menu = checkMouseMenu(position);
				updateMenu();
			}
			break;
		}
		case sf::Event::MouseButtonPressed:
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (checkMouseMenu(position))
				{
					menu = checkMouseMenu(position);
					menuSelect(game);
				}
			}
			break;
		}
		default: break;
		}
	}
}

int StateMenu::checkMouseMenu(sf::Vector2f pos)
{
	if (menuPlay.getGlobalBounds().contains(pos))
		return 1;
	else if (menuEdit.getGlobalBounds().contains(pos))
		return 2;
	else if (menuExit.getGlobalBounds().contains(pos))
		return 3;
	else
		return 0;
}

void StateMenu::menuUp()
{
	if (menu > 1)
		--menu;
	updateMenu();
}
void StateMenu::menuDown()
{
	if (menu < 3)
		++menu;
	updateMenu();
}

void StateMenu::updateMenu()
{
	menuPlay.setFillColor(sf::Color::White);
	menuEdit.setFillColor(sf::Color::White);
	menuExit.setFillColor(sf::Color::White);

	switch (menu)
	{
	case 1:
	{
		menuPlay.setFillColor(sf::Color::Yellow);
		break;
	}
	case 2:
	{
		menuEdit.setFillColor(sf::Color::Yellow);
		break;
	}
	case 3:
	{
		menuExit.setFillColor(sf::Color::Yellow);
		break;
	}
	default:
		break;
	}

}

void StateMenu::menuSelect(Game* game)
{
	switch (this->menu)
	{
	case 1:
	{
		// Play
		game->pushState(new StateLevels());
		break;
	}
	case 2:
	{
		// Edit
		game->pushState(new StateEdit());
		break;
	}
	case 3:
	{
		// Exit
		game->window.close();
		break;
	}
	default:
		break;
	}
}

void StateMenu::init(Game* game)
{
	sf::Vector2f pos = sf::Vector2f(game->window.getSize());
	this->view.setSize(pos);
	pos *= 0.5f;
	this->view.setCenter(pos);
}

StateMenu::StateMenu()
{
	font.loadFromFile("resources/verdanab.ttf");


	// track what menu item is highlighted
	menu = 1;


	// Create the menu text parts
	menuPlay.setFont(font);
	menuPlay.setCharacterSize(60);
	menuPlay.setString("PLAY");
	menuPlay.setPosition(280.f, 160.f);

	menuEdit.setFont(font);
	menuEdit.setCharacterSize(60);
	menuEdit.setString("EDIT");
	menuEdit.setPosition(280.f, 240.f);

	menuExit.setFont(font);
	menuExit.setCharacterSize(60);
	menuExit.setString("EXIT");
	menuExit.setPosition(280.f, 320.f);

	updateMenu();

}
