#pragma once

#include <SFML/Graphics.hpp>

#include "sprites.hpp"

enum class TileType { INACCESSIBLE, FLAT, WATERWAY, WATERSOURCE, VOID, BRIDGE, SAPLING, COAL };

class Tile
{
public:

	SpriteHelper sprthlpr;
	sf::Sprite sprite;

	TileType tiletype;

	int tileVersion;

	void draw(sf::RenderWindow& window, float dt);

	void update();

	// This constructor is probably gonna grow pretty big. Depending on how I store tile info.
	Tile(sf::Texture& texture, const std::vector<SpriteInfo>& sprinfs, const TileType tileType);
	Tile();
};