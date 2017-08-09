#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

#include "tile.hpp"
#include "textures.hpp"

class Level
{
private:
	// remember to change this to -1 in the play version constructor.
	int selectedTile;
public:
	unsigned int width;
	unsigned int height;

	std::map<TileType, Tile> tileBank;
	Textures textures;

	// describes the position of elements
	std::vector<Tile> tiles;
	// and also a vector of blocks

	// void updateDirection(TileType tileType); (I don't know if I need this function)

	void loadTextures(const std::string& filename);
	void loadTiles(const std::string& filename);
	void load(const std::string& filename, unsigned int width, unsigned int height);

	void save(const std::string& filename);

	void draw(sf::RenderWindow& window, float dt);

	void createBlankLevel();

	// comment this one out if 
	void changeTile(TileType tiletype);

	int selectTileByPos(sf::Vector2f pos);

	Level();
	Level(const std::string& filename, unsigned int width, unsigned int height);
};