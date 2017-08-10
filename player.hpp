#pragma once

#include <SFML/Graphics.hpp>

#include "sprites.hpp"

class Player
{
public:
	SpriteHelper sprthlpr;
	sf::Sprite sprite;

	int playerVersion;

	void update();

	void draw(sf::RenderWindow& window, float dt);

	// do I need a default constructor?
	Player(sf::Texture& texture, const std::vector<SpriteInfo>& sprinfs);

};