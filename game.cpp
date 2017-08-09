
#include <SFML/Graphics.hpp>

#include "game.hpp"
#include "state.hpp"

void Game::gameLoop()
{
	sf::Clock clock;
	float dt;

	while (this->window.isOpen())
	{
		dt = clock.restart().asSeconds();

		getState()->handleInput(this);
		getState()->update(this, dt);
		//this->window.clear(sf::Color::Black); this already happens in the draw functions
		getState()->draw(this, dt);
		this->window.display();
	}
}

State* Game::getState()
{
	if (this->states.empty()) return nullptr;
	return this->states.back();
}

void Game::changeState(State* state)
{
	if (!this->states.empty())
	{
		popState();
	}
	pushState(state);

	getState()->init(this);
	return;
}

void Game::pushState(State* state)
{
	this->states.push_back(state);
	getState()->init(this);
	return;
}

void Game::popState()
{
	delete this->states.back();
	this->states.pop_back();
}

Game::Game()
{
	this->window.create(sf::VideoMode(800, 600), "title");
	this->window.setFramerateLimit(60);
}

Game::~Game()
{
	while (!this->states.empty()) {
		popState();
	}
}