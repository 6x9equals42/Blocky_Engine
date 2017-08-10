#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

#include "tile.hpp"
#include "textures.hpp"
#include "entity.hpp"
#include "player.hpp"

enum class Direction { UP, DOWN, LEFT, RIGHT };

class Level
{
private:
	// remember to change this to -1 in the play version constructor.
	int selectedTile;
	int playerPos;
	int startTile;
	int exitTile;

	// internal input logic functions.
	bool isEntity(int position);
	bool isPushableEntity(int position, Direction direction);
	bool isWalkable(int position, Direction direction);
	bool canPush(int position, Direction direction);
	int nextTile(int position, Direction direction);
	void playerMove(Direction direction);

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
	// and the player;
	Player player;

	// void updateDirection(TileType tileType); (I don't know if I need this function)

	// rendering
	void draw(sf::RenderWindow& window, float dt);
	// rendering player (only for play)
	void drawPlayer(sf::RenderWindow& window, float dt);

	// managing the level
	void loadTextures(const std::string& filename);
	void loadTiles(const std::string& filename);
	void loadEntities(const std::string& filename);
	void loadPlayer();
	void load(const std::string& filename, unsigned int width, unsigned int height);
	void reset(); // this will need to change to actually reload the level instead of just resetting player poss

	void save(const std::string& filename);
	// this function only exists to easily generate blank things, will be removed.
	// Edit it's contents to create levels of different sizes.
	void createBlankLevel();

	// editing functions -> I could move these to a different class, might make organization better.
	void changeTile(TileType tiletype);
	int selectTileByPos(sf::Vector2f pos);
	void deleteEntity();
	void setEntity(EntityType entitytype);
	void cycleTileVersion();
	void cycleEntityVersion();

	// input logic functions. These could also maybe be an individual class.
	void input(Direction direction);
	


	Level();
	Level(const std::string& filename, unsigned int width, unsigned int height);
};