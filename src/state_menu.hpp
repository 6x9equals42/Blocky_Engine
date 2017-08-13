#pragma once

#include "state.hpp"

class StateMenu : public State
{
private:
	sf::View view;
	sf::Font font;
	sf::Text menuPlay;
	sf::Text menuEdit;
	sf::Text menuExit;
	int menu;
	void menuUp();
	void menuDown();
	void updateMenu();
	void menuSelect(Game* game);
	//int checkMouseMenu(sf::Vector2f pos);
	
public:
	virtual void draw(Game* game, const float dt);
	virtual void update(Game* game, const float dt);
	virtual void handleInput(Game* game);

	virtual void init(Game* game);



	StateMenu();
};