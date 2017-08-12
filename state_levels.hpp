#pragma once

#include <SFML/Graphics.hpp>

#include "state.hpp"
#include "sprites.hpp"

class LevelButton
{
public:
	SpriteHelper sprthlpr;

	sf::Sprite sprite;

	int level;

	void draw(sf::RenderWindow& window, float dt);

	void update();

	// This constructor is probably gonna grow pretty big. Depending on how I store tile info.
	LevelButton(sf::Texture& texture, SpriteInfo sprinf, SpriteInfo sprinf2, int level);
	LevelButton();
};

class StateLevels : public State
{
private:
	sf::View view;
	sf::Font font;
	sf::Text menuBack;
	sf::Text menuClear;
	std::vector<LevelButton> levelButtons;
	sf::Texture buttonTex;
	int level;
	int menu;
	void menuUp();
	void menuDown();
	void menuLeft();
	void menuRight();
	void updateMenu();
	void menuSelect(Game* game);

public:
	virtual void draw(Game* game, const float dt);
	virtual void update(Game* game, const float dt);
	virtual void handleInput(Game* game);

	virtual void init(Game* game);



	StateLevels();
};