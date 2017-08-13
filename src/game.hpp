#pragma once

#include <SFML/Graphics.hpp>
#include <vector>


class State;

class Game
{
private:
	
	
public:
	std::vector<State*> states;
	sf::RenderWindow window;

	void gameLoop();

	void pushState(State* state);

	void popState();

	void changeState(State* state);

	State* getState();

	Game();
	~Game();
};

