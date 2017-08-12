#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

#include "tile.hpp"
#include "textures.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "history.hpp"

enum class Direction { UP, DOWN, LEFT, RIGHT, NONE };

class Level
{
private:
	int selectedTile;
	int playerPos;
	int startTile;
	int exitTile;
	bool canExit;

	History levelHistory;

	// maps for use of water updating
	std::map<int, int> distanceUnsettled; // for dijkstra
	std::map<int, int> distanceSettled; 
	// and the recursive waterupdate function

	// history functions
	void stashLevelState();
	

	// internal input logic functions.
	bool onLand(int position);
	bool isEntity(int position);
	bool isSubEntity(int position);
	bool isPushableEntity(int position, Direction direction);
	bool isWalkable(int position, Direction direction);
	int nextTile(int position, Direction direction);
	void playerMove(Direction direction);
	void playerFace(Direction direction);
	void pushEntities(int position, Direction direction);
	void activateEntities(int position, Direction direction);
	// and boat logic
	bool isRowable(int position, Direction direction);
	bool isBoat(int position);
	bool isPushableBoat(int position, Direction direction);
	void pushBoats(int position, Direction direction);
	void boatMove(Direction direction);

public:
	unsigned int width;
	unsigned int height;
	std::string filename;

	std::map<TileType, Tile> tileBank;
	std::map<EntityType, Entity> entityBank;
	Textures textures;

	// describes the position of elements
	std::vector<Tile> tiles;
	// and also a vector of entities
	std::vector<Entity> entities;
	std::vector<Entity> subEntities;
	// and the player;
	Player player;
	// and a workaround using the player class
	Player exit;

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
	void reset(); 
	void reload();

	void save(const std::string& filename);
	void settleEntities();
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

	void undo();
	void clearHistory();
	
	void updateTrees();
	void updateWater();


	Level();
	Level(const std::string& filename, unsigned int width, unsigned int height);
};