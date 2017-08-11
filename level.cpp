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
		{ SpriteInfo(sf::Vector2i(128, 0)) }, 
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
		{ SpriteInfo(sf::Vector2i(0, 64)) }, 
			TileType::WATERWAY); // TODO Water Direction versions + empty + animations

	/*this->tileBank[TileType::DRAIN] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(64, 64)) }, 
			TileType::DRAIN);*/

	this->tileBank[TileType::COAL] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(128, 64)), SpriteInfo(sf::Vector2i(64, 192)) }, 
			TileType::COAL);

	this->tileBank[TileType::SAPLING] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(192, 64)), SpriteInfo(sf::Vector2i(0, 128)) }, 
			TileType::SAPLING);

	//this->tileBank[TileType::TREE] = Tile(textures.getTexture("spritesheet"), { SpriteInfo(sf::Vector2i(0, 128)) }, TileType::TREE);
}

void Level::loadEntities(const std::string& filename)
{
	this->entityBank[EntityType::BOAT] =
		Entity(textures.getTexture("spritesheet"),
		{ SpriteInfo(sf::Vector2i(64, 128)) },
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
		{ SpriteInfo(sf::Vector2i(0, 192)) },
			EntityType::ROCK);
}

void Level::loadPlayer()
{
	player = Player(textures.getTexture("playersheet"),
	{ SpriteInfo(sf::Vector2i(0, 0)) });
	playerPos = startTile;
}

void Level::settleEntities()
{
	std::vector<Entity>::iterator it = entities.begin();
	std::cout << "settling entities \n";
	std::cout << "before, we have " << subEntities.size() << " subentities \n";

	while (it != entities.end())
	{
		if (tiles[it->pos].tiletype == TileType::WATERWAY && !isWalkable(it->pos, Direction::NONE))
		{
			std::cout << "found one to settle at " << it->pos << "\n";
			subEntities.push_back(*it);
			it = entities.erase(it);
		}
		else
			++it;
	}	
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
	if (playerOnLand())
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
		// TODO for while on a boat. I'll do this logic later.
	}

}

bool Level::playerOnLand()
{
	if (tiles[playerPos].tiletype != TileType::WATERWAY)
		return true;
	for (auto entity : subEntities)
	{
		if (entity.pos == playerPos && entity.entityType == EntityType::ROCK)
			return true;
	}
	return false;
}

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
		(tiletype == TileType::WATERWAY && this->isSubEntity(newPosition)))
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
		(isEntity(position) && tiletype == TileType::WATERWAY && tileVersion == 0 && // TODO update the waterversion when i add more types
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
	// TODO Check that if a torch lands on a coal square, destroy the torch and light the coal.
	// TODO if entity ends in water, either set to subentity or delete. boats can go in empty waterways, but cannot move!

	// move the entity on the current tile.
	for (auto &entity : entities)
	{
		if (entity.pos == position)
		{
			entity.pos = newPosition;
			break;
		}
	}
}

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
			//std::cout << "INTIIALIZED";
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
			if (targetTile->tiletype == TileType::WATERWAY && !(isSubEntity(newPosition))) // TODO make sure this checks properly for which waterway types have water
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
			if (safeToCheck && (nextEnt->entityType == EntityType::BUCKET && nextEnt->entityVersion == 0)) // TODO make it so only pours 1 ahead at a time. (put activate call before.
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
			else if (targetTile->tiletype == TileType::SAPLING && targetTile->tileVersion == 0)  // TODO also make sure to account for different tileversinos here
			{
				//water the sapling
				// TODO add tree versions and change this one.
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
			else if (targetTile->tiletype == TileType::SAPLING && targetTile->tileVersion == 0)  // TODO ditto as above
			{
				// TODO change the version of the sapling tile.

				// if already a tree, make it burn!!!
				return;
			}
		}
		// lighting the current torch
		else if (currentEnt->entityVersion == 1)
		{
			if (safeToCheck && (nextEnt->entityType == EntityType::TORCH && nextEnt->entityVersion == 0))
			{
				//std::cout << "THIS PART IS CALLED \n";
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

Level::Level()
{
	selectedTile = -1;
}

Level::Level(const std::string& filename, unsigned int width, unsigned int height)
{
	
	selectedTile = -1;
	startTile = 58;
	exitTile = 63;
	load(filename, width, height);
}