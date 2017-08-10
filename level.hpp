#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

#include "tile.hpp"
#include "textures.hpp"
#include "entity.hpp"

class Level
{
private:
	// remember to change this to -1 in the play version constructor.
	int selectedTile;
public:
	unsigned int width;
	unsigned int height;

	std::map<TileType, Tile> tileBank;
	std::map<EntityType, Entity> entityBank;
	Textures textures;

	// describes the position of elements
	std::vector<Tile> tiles;
	// and also a vector of entities
	std::vector<Entity> entities;

	// void updateDirection(TileType tileType); (I don't know if I need this function)

	// rendering
	void draw(sf::RenderWindow& window, float dt);

	// managing the level
	void loadTextures(const std::string& filename);
	void loadTiles(const std::string& filename);
	void loadEntities(const std::string& filename);
	void loadPlayer();
	void load(const std::string& filename, unsigned int width, unsigned int height);

	void save(const std::string& filename);
	// this function only exists to easily generate blank things, will be removed.
	// Edit it's contents to create levels of different sizes.
	void createBlankLevel();

	// editing functions
	void changeTile(TileType tiletype);
	int selectTileByPos(sf::Vector2f pos);
	void deleteEntity();
	void setEntity(EntityType entitytype);



	Level();
	Level(const std::string& filename, unsigned int width, unsigned int height);
};