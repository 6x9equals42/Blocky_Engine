#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm> // need this to use vector remove

#include "level.hpp"
#include "tile.hpp"

#include <iostream> // TODO Remove this (and other ios before the end)

void Level::load(const std::string& filename, unsigned int width, unsigned int height)
{
	loadTextures(filename);
	loadTiles(filename);
	loadEntities(filename);
	loadPlayer();

	std::ifstream inputFile;
	inputFile.open(filename + ".level", std::ios::in | std::ios::binary);

	this->width = width;
	this->height = height;

	int numEntities;
	int numSubEntities;

	inputFile.read((char*)&startTile, sizeof(int));
	inputFile.read((char*)&exitTile, sizeof(int));

	// load entities
	inputFile.read((char*)&numEntities, sizeof(int));
	for (int index = 0; index < numEntities; ++index)
	{
		EntityType entitytype;
		inputFile.read((char*)&entitytype, sizeof(int));

		this->entities.push_back(entityBank.at(entitytype));

		Entity& entity = this->entities.back();
		inputFile.read((char*)&entity.entityVersion, sizeof(int));
		inputFile.read((char*)&entity.pos, sizeof(int));
	}

	// load subentities
	inputFile.read((char*)&numSubEntities, sizeof(int));
	for (int index = 0; index < numSubEntities; ++index)
	{
		EntityType entitytype;
		inputFile.read((char*)&entitytype, sizeof(int));

		this->subEntities.push_back(entityBank.at(entitytype));

		Entity& entity = this->subEntities.back();
		inputFile.read((char*)&entity.entityVersion, sizeof(int));
		inputFile.read((char*)&entity.pos, sizeof(int));
	}

	// load tiles
	for (int index = 0; index < int(this->width * this->height); ++index)
	{
		TileType tiletype;
		inputFile.read((char*)&tiletype, sizeof(int));
		
		this->tiles.push_back(tileBank.at(tiletype));

		Tile& tile = this->tiles.back();
		inputFile.read((char*)&tile.tileVersion, sizeof(int));
		
	}

	inputFile.close();

	return;
}

void Level::loadTextures(const std::string& filename)
{
	// TODO make this also load from file eventually (which textures to load)
	textures.loadTexture("spritesheet", "media/spritesheet.png");
	textures.loadTexture("playersheet", "media/playersheet.png");
}

void Level::loadTiles(const std::string& filename)
{
	this->tileBank[TileType::BRIDGE] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(128, 0)),   // water <- bridge ||
			SpriteInfo(sf::Vector2i(384, 64)),  // water /\, bridge =
			SpriteInfo(sf::Vector2i(448, 0)),   // water -> bridge ||
			SpriteInfo(sf::Vector2i(448, 64)),   // water \/ bridge =
			SpriteInfo(sf::Vector2i(512, 0)),   // water <- bridge =
			SpriteInfo(sf::Vector2i(512, 64)),   // water /\ bridge ||
			SpriteInfo(sf::Vector2i(512, 128)),   // water -> bridge =
			SpriteInfo(sf::Vector2i(512, 192)),   // water \/ bridge ||
			SpriteInfo(sf::Vector2i(384, 192)),  // no water, bridge =
			SpriteInfo(sf::Vector2i(448, 128)) },   // no water, bridge ||
			TileType::BRIDGE);

	this->tileBank[TileType::FLAT] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(64, 0)) }, 
			TileType::FLAT);

	this->tileBank[TileType::INACCESSIBLE] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(0, 0)) }, 
			TileType::INACCESSIBLE);

	this->tileBank[TileType::VOID] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(192, 0)) }, 
			TileType::VOID);

	this->tileBank[TileType::WATERWAY] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(0, 64)),      // <-
			SpriteInfo(sf::Vector2i(320, 64)),  // /\ 
			SpriteInfo(sf::Vector2i(320, 0)),   // ->
			SpriteInfo(sf::Vector2i(384, 0)),   // \/
			SpriteInfo(sf::Vector2i(448, 192)), // New Watersource 
			SpriteInfo(sf::Vector2i(384, 128)) },  // TODO Check to make sure all water < 4 becomes 5 if makes sense
			TileType::WATERWAY); // TODO  + animations

	// This is reimagined again. I think I will algorithmically determine direction
	// based on proximity to sources for all waterways. Should also be useful for practicing
	// writing algorithms.
	// deleted now because if I want to never change tiles in a level, only versions.
	/*this->tileBank[TileType::WATERSOURCE] =
		Tile(textures.getTexture("spritesheet"),
		{ SpriteInfo(sf::Vector2i(448, 192)) },
			TileType::WATERSOURCE); */

	/*this->tileBank[TileType::DRAIN] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(64, 64)) }, 
			TileType::DRAIN);*/

	this->tileBank[TileType::COAL] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(128, 64)), 
			SpriteInfo(sf::Vector2i(64, 192)) }, 
			TileType::COAL);

	this->tileBank[TileType::SAPLING] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(192, 64)), // basic sapling
			SpriteInfo(sf::Vector2i(256, 0)), // sapling with light
			SpriteInfo(sf::Vector2i(64, 64)), // sapling with water-next
			SpriteInfo(sf::Vector2i(64, 64)), // sapling with perma water
			SpriteInfo(sf::Vector2i(0, 128)), // sapling fully grown
			SpriteInfo(sf::Vector2i(256, 64)) }, // burned
			TileType::SAPLING);

	//this->tileBank[TileType::TREE] = Tile(textures.getTexture("spritesheet"), { SpriteInfo(sf::Vector2i(0, 128)) }, TileType::TREE);
}

void Level::loadEntities(const std::string& filename)
{
	this->entityBank[EntityType::BOAT] =
		Entity(textures.getTexture("spritesheet"),
		{ SpriteInfo(sf::Vector2i(64, 128)),
			SpriteInfo(sf::Vector2i(320, 128)),
			SpriteInfo(sf::Vector2i(320, 192)),
			SpriteInfo(sf::Vector2i(256, 192)) },
			EntityType::BOAT);
	this->entityBank[EntityType::BUCKET] =
		Entity(textures.getTexture("spritesheet"),
		{ SpriteInfo(sf::Vector2i(128, 128)), SpriteInfo(sf::Vector2i(128, 192)) },
			EntityType::BUCKET);
	this->entityBank[EntityType::TORCH] =
		Entity(textures.getTexture("spritesheet"),
		{ SpriteInfo(sf::Vector2i(192, 128)), SpriteInfo(sf::Vector2i(192, 192)) },
			EntityType::TORCH);
	this->entityBank[EntityType::ROCK] =
		Entity(textures.getTexture("spritesheet"),
		{ SpriteInfo(sf::Vector2i(0, 192)),
			SpriteInfo(sf::Vector2i(256, 128)) },
			EntityType::ROCK);
}

void Level::loadPlayer()
{
	player = Player(textures.getTexture("playersheet"),
	{ SpriteInfo(sf::Vector2i(0, 0)) });
	playerPos = startTile;

	exit = Player(textures.getTexture("spritesheet"),
	{ SpriteInfo(sf::Vector2i(576, 0)) });
}

void Level::settleEntities()
{
	std::vector<Entity>::iterator it = entities.begin();
	//std::cout << "settling entities \n";
	//std::cout << "before, we have " << subEntities.size() << " subentities \n";

	while (it != entities.end())
	{
		if ((tiles[it->pos].tiletype == TileType::WATERWAY) && 
			!isWalkable(it->pos, Direction::NONE))
		{
			//std::cout << "found one to settle at " << it->pos << "\n";
			if (it->entityType == EntityType::ROCK)
				it->entityVersion = 1;
			if (it->entityType == EntityType::BOAT && it->entityVersion == 0)
				it->entityVersion = 2;
			if (it->entityType == EntityType::BOAT && it->entityVersion == 1)
				it->entityVersion = 3;
			subEntities.push_back(*it);
			it = entities.erase(it);
		}
		else
			++it;
	}	
	updateWater();
}

void Level::save(const std::string& filename)
{
	settleEntities();
	std::ofstream outputFile;
	outputFile.open(filename + ".level", std::ios::out | std::ios::binary);

	int numEntities = this->entities.size();
	int numSubEntities = this->subEntities.size();

	outputFile.write((char*)&startTile, sizeof(int));
	outputFile.write((char*)&exitTile, sizeof(int));

	outputFile.write((char*)&numEntities, sizeof(int));

	for (auto entity : this->entities)
	{
		outputFile.write((char*)&entity.entityType, sizeof(int));
		outputFile.write((char*)&entity.entityVersion, sizeof(int));
		outputFile.write((char*)&entity.pos, sizeof(int));
	}
	outputFile.write((char*)&numSubEntities, sizeof(int));

	for (auto entity : this->subEntities)
	{
		outputFile.write((char*)&entity.entityType, sizeof(int));
		outputFile.write((char*)&entity.entityVersion, sizeof(int));
		outputFile.write((char*)&entity.pos, sizeof(int));
	}

	for (auto tile : this->tiles)
	{
		outputFile.write((char*)&tile.tiletype, sizeof(int));
		outputFile.write((char*)&tile.tileVersion, sizeof(int));
	}

	outputFile.close();

	return;
}

void Level::reload()
{
	tiles.clear();
	entities.clear();
	subEntities.clear();

	load(this->filename, this->width, this->height);
	updateTrees();
	stashLevelState();
}

void Level::reset()
{
	this->playerPos = this->startTile;
}

void Level::draw(sf::RenderWindow& window, float dt)
{
	for (int y = 0; y < int(this->height); ++y)
	{
		for (int x = 0; x < int(this->width); ++x)
		{
			sf::Vector2f pos;
			pos.x = x * 64;
			pos.y = y * 64;
			this->tiles[y*this->width + x].sprite.setPosition(pos);

			// then draw it
			if (y*this->width + x == selectedTile)
			{
				this->tiles[y*this->width + x].sprite.setColor(sf::Color(255,255,150));
			}
			else
			{
				this->tiles[y*this->width + x].sprite.setColor(sf::Color::White);
			}

			this->tiles[y*this->width + x].draw(window, dt);
		}
	}
	// go through each subentity and draw at the right position.
	for (auto entity : this->subEntities)
	{
		sf::Vector2f spritePos;
		spritePos.x = (entity.pos % this->width) * 64;
		spritePos.y = int(entity.pos / this->width) * 64;
		entity.sprite.setPosition(spritePos);

		entity.draw(window, dt);
	}
	// draw the exit (so can be visible in editing too)
	if (canExit)
	{
		sf::Vector2f pos;
		pos.x = (exitTile % this->width) * 64;
		pos.y = int(exitTile / this->width) * 64;
		exit.sprite.setPosition(pos);
		exit.draw(window, dt);
	}

	// go through each entity and draw at the right position.
	for (auto entity : this->entities)
	{
		sf::Vector2f spritePos;
		spritePos.x = (entity.pos % this->width) * 64;
		spritePos.y = int(entity.pos / this->width) * 64;
		entity.sprite.setPosition(spritePos);

		entity.draw(window, dt);
	}
}

void Level::drawPlayer(sf::RenderWindow& window, float dt)
{
	// TODO add extra stuff for a moving animation eventually
	if (playerPos > -1)
	{
		sf::Vector2f pos;
		pos.x = (playerPos % this->width) * 64;
		pos.y = int(playerPos / this->width) * 64;
		player.sprite.setPosition(pos);
		player.draw(window, dt);
	}
}

int Level::selectTileByPos(sf::Vector2f pos)
{
	if (pos.x < 0 || pos.x > this->width * 64 || pos.y < 0 || pos.y > this->height * 64)
	{
		selectedTile = -1;
	}
	else
	{
		selectedTile = int(pos.x / 64) + int(pos.y / 64) * this->width;
	}
	return selectedTile;
}

void Level::changeTile(TileType tiletype)
{
	if (selectedTile > -1 && tiletype != tiles[selectedTile].tiletype)
	{
		this->tiles.insert(tiles.begin() + selectedTile, tileBank.at(tiletype));
		this->tiles.erase(tiles.begin() + selectedTile + 1);
	}
}

void Level::cycleTileVersion()
{
	int numVersions = this->tiles[selectedTile].sprthlpr.getNumVersions();

	this->tiles[selectedTile].tileVersion = (this->tiles[selectedTile].tileVersion + 1) % numVersions;
}

void Level::setEntity(EntityType entitytype)
{
	this->entities.push_back(entityBank.at(entitytype));
	this->entities.back().pos = selectedTile;
}

void Level::deleteEntity()
{
	// this is an idiom I should memorize.
	entities.erase(std::remove_if(entities.begin(), entities.end(), 
		[&](const Entity entity)->bool{ return entity.pos == selectedTile; }),  // would you look at that, a lambda!
		entities.end());
	subEntities.erase(std::remove_if(subEntities.begin(), subEntities.end(),
		[&](const Entity entity)->bool { return entity.pos == selectedTile; }), 
		subEntities.end());
}

void Level::cycleEntityVersion()
{
	int numVersions = this->entities.back().sprthlpr.getNumVersions();
	this->entities.back().entityVersion = (this->entities.back().entityVersion + 1) % numVersions;

}

void Level::createBlankLevel()
{
	loadTextures("");
	loadTiles("");
	this->width = 8;
	this->height = 8;
	for (int index = 0; index < 64; ++index)
	{
		this->tiles.push_back(tileBank.at(TileType::INACCESSIBLE));
	}
	selectedTile = -1;
}

// input handling functions
int Level::nextTile(int position, Direction direction)
{
	switch (direction)
	{
	case Direction::UP:
	{
		if (position < this->width)
			return -1;
		return position - this->width;
	}
	case Direction::DOWN:
	{
		if (position >= this->width * (this->height - 1))
			return -1;
		return position + this->width;
	}
	case Direction::LEFT:
	{
		if (position % this->width == 0)
			return -1;
		return position - 1;
	}
	case Direction::RIGHT:
	{
		if (position % this->width == -1 % this->width)
			return -1;
		return position + 1;
	}
	case Direction::NONE:
	{
		return position;
	}
	default:
		return -1;
	}
}

void Level::playerMove(Direction direction)
{
	playerPos = nextTile(playerPos, direction);
}

void Level::playerFace(Direction directio)
{
	// TODO this will be player version change.
}

void Level::input(Direction direction)
{
	/////////////// My ideas for rn (this is just for on land!!)
	// check if it's walkable
		// if it's walkable, check if there's an entity next
			// yes, then check for pushable entity
				// if yes, push function(pushes all recursively), face
				// if not, activate recursively, face
			// no, then walk, face
		// if not, then just face, don't move
	if (onLand(playerPos))
	{
		if (isWalkable(playerPos, direction))
		{
			//std::cout << "I can walk...";
			if (isEntity(nextTile(playerPos, direction)))
			{
				//std::cout << "There is an entity...";
				if (isPushableEntity(playerPos, direction))
				{
					//std::cout << "I can push it \n";
					playerFace(direction);
					pushEntities(playerPos, direction);
					settleEntities();
					playerMove(direction);
				}
				else
				{
					//std::cout << "I cannot push it \n";
					playerFace(direction);
					activateEntities(nextTile(playerPos, direction), direction);
				}
			}
			else
			{
				//std::cout << "It is blank\n";
				playerMove(direction);
				playerFace(direction);
			}
		}
		else
		{
			//std::cout << "not walkable \n";
			playerFace(direction);
		}
	}
	else if (isBoat(playerPos))
	{
		/////////////// 2nd batch of ideas
		// for side of boat
			// check if it's walkable only to the sides of the boat
				// if it's walkable, check if there's an entity next
					// yes, then check for pushable entity
						// if yes, push function(pushes all recursively), face
						// if not, activate recursively, face
					// no, then walk, face
				// if not, face nothing
		// for end of boat
			// check if there is watertile (not with a subbed rock) ** need this function isRowable
				// if yes check if there is a boat ** isBoat
					// yes, then check if boat is pushable ** isBoatPushable
						// if yes, push (pushes all boats along + the entities on them + player), face ** pushBoats
						// if not, face. cannot activate.
					// no, move the boat, move the player ** moveBoat, movePlayer **
				// if not, face

		// get the direction the boat is facing
		int boatDir;
		for (auto entity : subEntities)
		{
			if (entity.pos == playerPos)
				boatDir = entity.entityVersion;
		}

		if (((direction == Direction::UP || direction == Direction::DOWN) && boatDir == 2) ||
			((direction == Direction::LEFT || direction == Direction::RIGHT) && boatDir == 3))
		{
			if (isWalkable(playerPos, direction))
			{
				//std::cout << "I can walk...";
				if (isEntity(nextTile(playerPos, direction)))
				{
					//std::cout << "There is an entity...";
					if (isPushableEntity(playerPos, direction))
					{
						//std::cout << "I can push it \n";
						playerFace(direction);
						pushEntities(playerPos, direction);
						settleEntities();
						playerMove(direction);
					}
					else
					{
						//std::cout << "I cannot push it \n";
						playerFace(direction);
						activateEntities(nextTile(playerPos, direction), direction);
					}
				}
				else
				{
					//std::cout << "It is blank\n";
					playerMove(direction);
					playerFace(direction);
				}
			}
			else
			{
				//std::cout << "not walkable \n";
				playerFace(direction);
			}
		}
		else
		{
			if (isRowable(playerPos, direction))
			{
				if (isBoat(nextTile(playerPos, direction)))
				{
					if (isPushableBoat(playerPos, direction))
					{
						pushBoats(nextTile(playerPos, direction), direction);
						playerFace(direction);
						boatMove(direction);
					}
					else
					{
						playerFace(direction);
					}
				}
				else
				{
					playerFace(direction);
					boatMove(direction);
				}
			}
			else
			{
				playerFace(direction);
			}
		}
	}
	updateTrees();

	stashLevelState();
}

bool Level::onLand(int position)
{
	if (tiles[position].tiletype != TileType::WATERWAY)
		return true;
	for (auto entity : subEntities)
	{
		if (entity.pos == position && entity.entityType == EntityType::ROCK)
			return true;
	}
	return false;
}

// check if any boats / rocks in a waterway tile at the position indicated
// actually assumes this will only be called on a proper (uncorrupted) waterway
bool Level::isSubEntity(int position)
{
	for (auto entity : subEntities)
	{
		if (entity.pos == position)
			return true;
	}
	return false;
}

bool Level::isEntity(int position)
{
	for (auto entity : entities)
	{
		if (entity.pos == position)
			return true;
	}
	return false;
}

// call from the current player's position
bool Level::isWalkable(int position, Direction direction)
{
	int newPosition = this->nextTile(position, direction);
	
	if (newPosition == -1)
		return false;

	TileType tiletype = this->tiles[newPosition].tiletype;
	int tileVersion = this->tiles[newPosition].tileVersion;

	if (tiletype == TileType::BRIDGE ||
		(tiletype == TileType::COAL && tileVersion == 0) ||
		tiletype == TileType::FLAT ||
		((tiletype == TileType::WATERWAY) && this->isSubEntity(newPosition)))
	{
		return true;
	}
	return false;
}

// call from player position initially
bool Level::isPushableEntity(int position, Direction direction)
{
	int newPosition = this->nextTile(position, direction);

	if (newPosition == -1)
		return false;

	TileType tiletype = this->tiles[newPosition].tiletype;
	int tileVersion = this->tiles[newPosition].tileVersion;

	EntityType entityType;
	for (auto entity : entities)
	{
		if (entity.pos == position)
		{
			entityType = entity.entityType;
		}
	}

	if (tiletype == TileType::INACCESSIBLE ||
		tiletype == TileType::SAPLING ||
		(tiletype == TileType::COAL && tileVersion == 1) ||
		(isEntity(position) && (tiletype == TileType::WATERWAY) &&
			(entityType == EntityType::BUCKET || entityType == EntityType::TORCH)) &&
			!this->isSubEntity(newPosition)) 
	{
		return false;
	}
	
	for (auto entity : entities)
	{
		if (entity.pos == newPosition)
		{
			// there is an entity in the next tile!
			return isPushableEntity(newPosition, direction);
		}
			
	}
	return true;
}

void Level::pushEntities(int position, Direction direction)
{
	//std::cout << "pushing..\n";
	int newPosition = this->nextTile(position, direction);

	// call push on the entity in the next tile.
	for (auto entity : entities)
	{
		if (entity.pos == newPosition)
		{
			//std::cout << "the entity pushes another\n";
			pushEntities(newPosition, direction);
			break;
		}
	}
	bool remove = false;
	// move the entity on the current tile, destroyo torches and light the coals if relevant
	for (auto &entity : entities)
	{
		if (entity.pos == position)
		{
			entity.pos = newPosition;
			if (tiles[newPosition].tiletype == TileType::COAL && entity.entityType == EntityType::TORCH && entity.entityVersion == 0)
			{
				tiles[newPosition].tileVersion = 1;
				remove = true;
			}
			if (entity.entityType == EntityType::BOAT)
			{
				if (direction == Direction::LEFT || direction == Direction::RIGHT)
					entity.entityVersion = 0;
				else
					entity.entityVersion = 1;
			}
			break;
		}
	}
	if (remove)
	{
		entities.erase(std::remove_if(entities.begin(), entities.end(),
			[&](const Entity entity)->bool { return entity.pos == newPosition; }),
			entities.end());
	}

	// delete entities that fall into the void
	if (tiles[newPosition].tiletype == TileType::VOID)
	{
		entities.erase(std::remove_if(entities.begin(), entities.end(),
			[&](const Entity entity)->bool { return entity.pos == newPosition; }),
			entities.end());
		subEntities.erase(std::remove_if(subEntities.begin(), subEntities.end(),
			[&](const Entity entity)->bool { return entity.pos == newPosition; }),
			subEntities.end());
	}

	
}

// activate the abliities of the entities you attempted to push
void Level::activateEntities(int position, Direction direction)
{
	int newPosition = this->nextTile(position, direction);
	Entity* currentEnt;
	Entity* nextEnt;
	if (newPosition == -1)
		return;

	Tile* targetTile = &tiles[newPosition];

	// get the references of both entities
	for (int index = 0; index < entities.size(); ++index)
	{
		if (entities[index].pos == position)
		{
			currentEnt = &entities[index];
		}
		if (entities[index].pos == newPosition)
		{
			nextEnt = &entities[index];
		}
	}

	bool safeToCheck = isEntity(newPosition);

	// calling activate is interspersed within the conditionals to control chain behavior

	if (currentEnt->entityType == EntityType::BUCKET)
	{
		// filling a bucket
		if (currentEnt->entityVersion == 0)
		{
			// from a full bucket
			// it makes sense that buckets pour in a line instead of scoop.
			/*if (nextEnt->entityType == EntityType::BUCKET && nextEnt->entityVersion == 1)
			{
				// change the bucket versions
			}*/
			if ((targetTile->tiletype == TileType::WATERWAY) && 
				targetTile->tileVersion < 5 && !(isSubEntity(newPosition)))
			{
				// fill the one bucket
				currentEnt->entityVersion = 1;
				// don't need to activate cause its the end
				return;
			}
		}
		// emptying a bucket
		else if (currentEnt->entityVersion == 1)
		{
			// passing to next bucket
			if (safeToCheck && (nextEnt->entityType == EntityType::BUCKET && nextEnt->entityVersion == 0))
			{
				//activate next
				activateEntities(newPosition, direction);
				// pour to next bucket
				currentEnt->entityVersion = 0;
				nextEnt->entityVersion = 1;
				return;
			}
			// putting out a torch
			else if (safeToCheck && (nextEnt->entityType == EntityType::TORCH && nextEnt->entityVersion == 0))
			{
				// put out torch
				nextEnt->entityVersion = 1;
				currentEnt->entityVersion = 0;
				// then activate
				activateEntities(newPosition, direction);
				return;
			}
			// putting out a fire
			else if (targetTile->tiletype == TileType::COAL && targetTile->tileVersion == 1)
			{
				//put out the fire
				targetTile->tileVersion = 0;
				currentEnt->entityVersion = 0;
				// don't need to acctivate 'cause it's the end.
				return;
			}
			// watering a sapling
			else if (targetTile->tiletype == TileType::SAPLING && 
				(targetTile->tileVersion < 3))
			{
				//water the sapling
				targetTile->tileVersion = 3;

				currentEnt->entityVersion = 0;
				// don't need to acctivate 'cause it's the end.
				return;
			}
		}
	}
	else if (currentEnt->entityType == EntityType::TORCH)
	{
		// using the current torch
		if (currentEnt->entityVersion == 0)
		{
			if (safeToCheck && nextEnt->entityType == EntityType::TORCH)
			{
				// activate the next first before making sure the next is on
				activateEntities(newPosition, direction);
				nextEnt->entityVersion = 0;
				return;
			}
			// using it on a sapling
			else if (targetTile->tiletype == TileType::SAPLING) 
			{
				targetTile->tileVersion = 5;
				// if already a tree, make it burn!!!
				return;
			}
			// dousing it
			else if ((targetTile->tiletype == TileType::WATERWAY) && 
				targetTile->tileVersion < 5 && !(isSubEntity(newPosition)))
			{
				currentEnt->entityVersion = 1;
			}
		}
		// lighting the current torch
		else if (currentEnt->entityVersion == 1)
		{
			if (safeToCheck && (nextEnt->entityType == EntityType::TORCH && nextEnt->entityVersion == 0))
			{
				// order doesn't matter here, so keep it the same as copy/paste
				activateEntities(newPosition, direction);
				currentEnt->entityVersion = 0;
				return;
			}
			else if (targetTile->tiletype == TileType::COAL && targetTile->tileVersion == 1)
			{
				currentEnt->entityVersion = 0;
				return;
			}

		}
	}
	if (safeToCheck)
	{
		activateEntities(newPosition, direction);
	}
}

bool Level::isRowable(int position, Direction direction)
{
	int newPosition = this->nextTile(position, direction);

	if (newPosition == -1)
		return false;

	TileType tiletype = this->tiles[newPosition].tiletype;
	int tileVersion = this->tiles[newPosition].tileVersion;

	bool isRock = false;
	for (auto entity : subEntities)
	{
		if (entity.pos == newPosition && entity.entityType == EntityType::ROCK)
		{
			isRock = true;
			break;
		}
	}

	if ((tiletype == TileType::WATERWAY) && !isRock && tileVersion < 5)
	{
		return true;
	}
	return false;
}

bool Level::isBoat(int position)
{
	for (auto entity : subEntities)
	{
		if (entity.pos == position && entity.entityType == EntityType::BOAT)
		{
			return true;
		}
	}
	return false;
}

bool Level::isPushableBoat(int position, Direction direction)
{
	int newPosition = this->nextTile(position, direction);

	if (newPosition == -1)
		return false;

	TileType tiletype = this->tiles[newPosition].tiletype;
	int tileVersion = this->tiles[newPosition].tileVersion;

	EntityType entityType;
	for (auto entity : subEntities)
	{
		if (entity.pos == position)
		{
			entityType = entity.entityType;
		}
	}

	if ((tiletype != TileType::WATERWAY && tiletype != TileType::VOID) ||
		(isSubEntity(position) && entityType == EntityType::ROCK))
	{
		//std::cout << "cannot push";
		return false;
	}

	for (auto entity : subEntities)
	{
		if (entity.pos == newPosition)
		{
			// there is an entity in the next tile!
			return isPushableBoat(newPosition, direction);
		}
	}
	return true;
}

void Level::pushBoats(int position, Direction direction)
{
	int newPosition = this->nextTile(position, direction);

	// call push on the entity in the next tile.
	for (auto entity : subEntities)
	{
		if (entity.pos == newPosition)
		{
			pushBoats(newPosition, direction);
			break;
		}
	}

	// move the entity on the current tile. 
	for (auto &entity : entities)
	{
		if (entity.pos == position)
		{
			entity.pos = newPosition;
			break;
		}
	}
	for (auto &entity : subEntities)
	{
		if (entity.pos == position)
		{
			entity.pos = newPosition;
			break;
		}
	}
	if (tiles[newPosition].tiletype == TileType::VOID)
	{
		entities.erase(std::remove_if(entities.begin(), entities.end(),
			[&](const Entity entity)->bool { return entity.pos == newPosition; }),
			entities.end());
		subEntities.erase(std::remove_if(subEntities.begin(), subEntities.end(),
			[&](const Entity entity)->bool { return entity.pos == newPosition; }),
			subEntities.end());
	}
}

void Level::boatMove(Direction direction)
{
	for (auto &entity : subEntities)
	{
		if (entity.pos == playerPos)
		{
			entity.pos = nextTile(playerPos, direction);
			break;
		}
	}
	playerPos = nextTile(playerPos, direction);
}

void Level::updateTrees()
{
	bool allAreGrown = true;

	for (int index = 0; index < this->tiles.size(); ++index)
	{
		if (this->tiles[index].tiletype == TileType::SAPLING)
		{
			if (this->tiles[index].tileVersion == 4) {}
			else if (this->tiles[index].tileVersion == 5)
				allAreGrown = false;
			else
			{
				// determine whether there is light
				bool hasLight = false;
				if ((nextTile(index, Direction::LEFT) != -1 &&
					this->tiles[nextTile(index, Direction::LEFT)].tiletype == TileType::COAL &&
					this->tiles[nextTile(index, Direction::LEFT)].tileVersion == 1) ||
					(nextTile(index, Direction::RIGHT) != -1 && 
						this->tiles[nextTile(index, Direction::RIGHT)].tiletype == TileType::COAL &&
						this->tiles[nextTile(index, Direction::RIGHT)].tileVersion == 1) ||
						(nextTile(index, Direction::UP) != -1 && 
							this->tiles[nextTile(index, Direction::UP)].tiletype == TileType::COAL &&
							this->tiles[nextTile(index, Direction::UP)].tileVersion == 1) ||
							(nextTile(index, Direction::DOWN) != -1 && 
								this->tiles[nextTile(index, Direction::DOWN)].tiletype == TileType::COAL &&
								this->tiles[nextTile(index, Direction::DOWN)].tileVersion == 1))
				{
					hasLight = true;
				}
				else
				{
					for (auto entity : this->entities)
					{
						if (entity.entityType == EntityType::TORCH)
						{
							if (entity.pos == nextTile(index, Direction::LEFT) ||
								entity.pos == nextTile(index, Direction::RIGHT) ||
								entity.pos == nextTile(index, Direction::UP) ||
								entity.pos == nextTile(index, Direction::DOWN))
							{
								hasLight = true;
								break;
							}
						}
					}
				}

				// determine if there is an adjacent water tile
				bool nextToWater = false;
				if ((isRowable(index, Direction::LEFT)) ||
					(isRowable(index, Direction::RIGHT)) ||
					(isRowable(index, Direction::UP)) ||
					(isRowable(index, Direction::DOWN)))
				{
					nextToWater = true;
				}

				if (hasLight)
				{
					if (nextToWater || this->tiles[index].tileVersion == 3)
						tiles[index].tileVersion = 4;
					else
						tiles[index].tileVersion = 1;
				}
				else
				{
					if (this->tiles[index].tileVersion == 3)
						tiles[index].tileVersion = 3;
					else if (nextToWater)
						this->tiles[index].tileVersion = 2;
					else
						this->tiles[index].tileVersion = 0;
				}
				if (tiles[index].tileVersion != 4)
					allAreGrown = false;
			}
		}
	}
	if (allAreGrown)
		canExit = true;
	else
		canExit = false;
}

// A very slightly modified implementation of dijkstra's algorithm
void Level::updateWater()
{
	// change all the normal waterways to type 0, && empty all bridges (maybe dont need to empty bridges, idk)
	for (auto &tile : this->tiles)
	{
		if (tile.tiletype == TileType::WATERWAY && tile.tileVersion != 4)
			tile.tileVersion = 0;
		if (tile.tiletype == TileType::BRIDGE &&
			(tile.tileVersion == 0 || tile.tileVersion == 2 || tile.tileVersion == 5 || tile.tileVersion == 7))
			tile.tileVersion = 9;
		if (tile.tiletype == TileType::BRIDGE &&
			(tile.tileVersion == 1 || tile.tileVersion == 3 || tile.tileVersion == 4 || tile.tileVersion == 6))
			tile.tileVersion = 8;
	}
	
	// kill all sources that have rocks in them
	for (auto entity : this->subEntities)
	{
		if (entity.entityType == EntityType::ROCK)
		{
			tiles[entity.pos] = this->tileBank.at(TileType::WATERWAY);
			tiles[entity.pos].tileVersion = 5;
		}
	}

	// have a 2 maps, one for settled elements & 1 for unsettled elements.
	distanceSettled.clear();
	distanceUnsettled.clear();

	// add all the waterways/bridges tiles to the unsettled map, with an unrealistically high distance value (1000)
	// add all watersources to the same unsettled map, with distance value = 0;
	for (int index = 0; index < this->tiles.size(); ++index)
	{
		if ((tiles[index].tiletype == TileType::WATERWAY && tiles[index].tileVersion == 0) ||
			tiles[index].tiletype == TileType::BRIDGE)
		{
			distanceUnsettled[index] = 1000;
		}
		if (tiles[index].tiletype == TileType::WATERWAY && tiles[index].tileVersion == 4)
		{
			distanceUnsettled[index] = 1; // don't want 0, because then we mistake tiles not in the map for ones in the map
		}
	}

	// while there are still unsettled nodes:
	while (!distanceUnsettled.empty())
	{
		// get the smallest in the map:
		std::pair<int, int> min = *std::min_element(distanceUnsettled.begin(), distanceUnsettled.end(),
		[](auto& l, auto& r) -> bool { return l.second < r.second; });
		// if all that are left are 1000, we're done, those are unreachable.
		if (min.second == 1000)
			break;

		// remove the element from unsettled
		distanceUnsettled.erase(min.first);
		// and add it to the settled array
		distanceSettled[min.first] = min.second;

		// for every unsettled tile that can be reached from the chosen tile, 
		// calculate a new distance to those tiles from the current tile.

		// if the adjacent tile is in the unsettled map
		int next = nextTile(min.first, Direction::LEFT);
		// I would have written a function for this part, but I felt there were too many functions already,
		// so it just gets written out 4 times, meh.
		if (distanceUnsettled.count(next))
		{
			if (distanceUnsettled[next] > min.second + 1)
				distanceUnsettled[next] = min.second + 1;
		}
		next = nextTile(min.first, Direction::RIGHT);
		if (distanceUnsettled.count(next))
		{
			if (distanceUnsettled[next] > min.second + 1)
				distanceUnsettled[next] = min.second + 1;
		}
		next = nextTile(min.first, Direction::UP);
		if (distanceUnsettled.count(next))
		{
			if (distanceUnsettled[next] > min.second + 1)
				distanceUnsettled[next] = min.second + 1;
		}
		next = nextTile(min.first, Direction::DOWN);
		if (distanceUnsettled.count(next))
		{
			if (distanceUnsettled[next] > min.second + 1)
				distanceUnsettled[next] = min.second + 1;
		}
	}
	
	// then calculate tiletype based on distances
	for (std::pair<int,int> position : distanceUnsettled)
	{
		if (tiles[position.first].tiletype == TileType::WATERWAY)
			tiles[position.first].tileVersion = 5;
		if (tiles[position.first].tiletype == TileType::BRIDGE)
		{
			// do nothing
		}
	}
	// and I don't see how to do this part w/o a ton of if statements... ugh
	for (std::pair<int, int> position : distanceSettled)
	{
		if (tiles[position.first].tiletype == TileType::WATERWAY && tiles[position.first].tileVersion != 4)
		{
			if (distanceSettled[nextTile(position.first, Direction::LEFT)] == position.second - 1)
				tiles[position.first].tileVersion = 2;
			else if (distanceSettled[nextTile(position.first, Direction::RIGHT)] == position.second - 1)
				tiles[position.first].tileVersion = 0;
			else if (distanceSettled[nextTile(position.first, Direction::UP)] == position.second - 1)
				tiles[position.first].tileVersion = 3;
			else if (distanceSettled[nextTile(position.first, Direction::DOWN)] == position.second - 1)
				tiles[position.first].tileVersion = 1;
		}
		if (tiles[position.first].tiletype == TileType::BRIDGE && tiles[position.first].tileVersion == 8)
		{
			if (distanceSettled[nextTile(position.first, Direction::LEFT)] == position.second - 1)
				tiles[position.first].tileVersion = 6;
			else if (distanceSettled[nextTile(position.first, Direction::RIGHT)] == position.second - 1)
				tiles[position.first].tileVersion = 4;
			else if (distanceSettled[nextTile(position.first, Direction::UP)] == position.second - 1)
				tiles[position.first].tileVersion = 1;
			else if (distanceSettled[nextTile(position.first, Direction::DOWN)] == position.second - 1)
				tiles[position.first].tileVersion = 3;
		}
		if (tiles[position.first].tiletype == TileType::BRIDGE && tiles[position.first].tileVersion == 9)
		{
			if (distanceSettled[nextTile(position.first, Direction::LEFT)] == position.second - 1)
				tiles[position.first].tileVersion = 2;
			else if (distanceSettled[nextTile(position.first, Direction::RIGHT)] == position.second - 1)
				tiles[position.first].tileVersion = 0;
			else if (distanceSettled[nextTile(position.first, Direction::UP)] == position.second - 1)
				tiles[position.first].tileVersion = 5;
			else if (distanceSettled[nextTile(position.first, Direction::DOWN)] == position.second - 1)
				tiles[position.first].tileVersion = 7;
		}
	}

}

void Level::stashLevelState()
{
	LevelState levelState;
	for (auto tile : this->tiles)
	{
		levelState.tileVersions.push_back(tile.tileVersion);
	}

	levelState.entities = this->entities;
	levelState.subEntities = this->subEntities;

	levelState.playerPos = this->playerPos;

	// I don't know why using the == directly on the vectors kept throwing compilation errors.
	// I tried every kind of overloading on the entity == and equal_to operators, but this works,
	// so I'll do this (even if it's ugly)
	bool same = true;
	for (int index = 0; index < levelState.entities.size(); ++index)
	{
		if (!(levelState.entities[index] == levelHistory.getLevel().entities[index]))
			same = false;
	}
	for (int index = 0; index < levelState.subEntities.size(); ++index)
	{
		if (!(levelState.subEntities[index] == levelHistory.getLevel().subEntities[index]))
			same = false;
	}
	for (int index = 0; index < levelState.tileVersions.size(); ++index)
	{
		if (!(levelState.tileVersions[index] == levelHistory.getLevel().tileVersions[index]))
			same = false;
	}
	if (levelState.playerPos != levelHistory.getLevel().playerPos)
		same = false;
	if (!same)
		levelHistory.pushLevel(levelState);
}

void Level::undo()
{
	LevelState levelState = levelHistory.popLevel();
	for (int index = 0; index < tiles.size(); ++index)
	{
		tiles[index].tileVersion = levelState.tileVersions[index];
	}
	this->entities = levelState.entities;
	this->subEntities = levelState.subEntities;

	this->playerPos = levelState.playerPos;


	updateWater();
	updateTrees();
}

void Level::clearHistory()
{
	/* intialize undo history */
	LevelState levelState;
	for (auto tile : this->tiles)
	{
		levelState.tileVersions.push_back(tile.tileVersion);
	}
	levelState.entities = this->entities;
	levelState.subEntities = this->subEntities;
	levelState.playerPos = this->playerPos;
	levelHistory = History(levelState);
	updateWater();
	updateTrees();
}

Level::Level()
{
	selectedTile = -1;
}

Level::Level(const std::string& filename, unsigned int width, unsigned int height)
{
	this->filename = filename;
	selectedTile = -1;
	startTile = 58;
	exitTile = 63;
	load(filename, width, height);

	clearHistory();

}