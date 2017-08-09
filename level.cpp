#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "level.hpp"
#include "tile.hpp"

void Level::load(const std::string& filename, unsigned int width, unsigned int height)
{
	loadTextures(filename);
	loadTiles(filename);

	std::ifstream inputFile;
	inputFile.open(filename + ".level", std::ios::in | std::ios::binary);

	this->width = width;
	this->height = height;

	for (int index = 0; index < int(this->width * this->height); ++index)
	{
		TileType tiletype;
		inputFile.read((char*)& tiletype, sizeof(int));
		
		this->tiles.push_back(tileBank.at(tiletype));

		Tile& tile = this->tiles.back();
		inputFile.read((char*)& tile.tileVersion, sizeof(int));
		
	}

	inputFile.close();

	return;
}

void Level::loadTextures(const std::string& filename)
{
	// make this also load from file eventually (which textures to load)
	textures.loadTexture("spritesheet", "media/spritesheet.png");
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

	this->tileBank[TileType::WATERSOURCE] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(192, 0)) }, 
			TileType::WATERSOURCE);

	this->tileBank[TileType::WATERWAY] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(0, 64)) }, 
			TileType::WATERWAY);

	this->tileBank[TileType::DRAIN] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(64, 64)) }, 
			TileType::DRAIN);

	this->tileBank[TileType::COAL] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(128, 64)) }, 
			TileType::COAL);

	this->tileBank[TileType::SAPLING] = 
		Tile(textures.getTexture("spritesheet"), 
		{ SpriteInfo(sf::Vector2i(192, 64)), SpriteInfo(sf::Vector2i(0, 128)) }, 
			TileType::SAPLING);

	//this->tileBank[TileType::TREE] = Tile(textures.getTexture("spritesheet"), { SpriteInfo(sf::Vector2i(0, 128)) }, TileType::TREE);
}

void Level::save(const std::string& filename)
{
	std::ofstream outputFile;
	outputFile.open(filename + ".level", std::ios::out | std::ios::binary);

	for (auto tile : this->tiles)
	{
		outputFile.write((char*)&tile.tiletype, sizeof(int));
		outputFile.write((char*)&tile.tileVersion, sizeof(int));
	}

	outputFile.close();

	return;
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
	if (tiletype != tiles[selectedTile].tiletype && selectedTile > -1)
	{
		this->tiles.insert(tiles.begin() + selectedTile, tileBank.at(tiletype));
		this->tiles.erase(tiles.begin() + selectedTile + 1);
	}
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

Level::Level()
{
	selectedTile = -1;
}

Level::Level(const std::string& filename, unsigned int width, unsigned int height)
{
	load(filename, width, height);
	selectedTile = -1;
}