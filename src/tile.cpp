#include <SFML/Graphics.hpp>

#include "tile.hpp"

void Tile::draw(sf::RenderWindow& window, float dt)
{
	this->sprthlpr.changeVersion(this->tileVersion);

	this->sprthlpr.update(dt);

	this->sprite.setTextureRect(this->sprthlpr.getBounds());

	window.draw(this->sprite);
}

void Tile::update()
{
	// put in the logic to change tile variants here. Maybe it should take a parameter
}


Tile::Tile(sf::Texture& texture, const std::vector<SpriteInfo>& sprinfs, const TileType tiletype)
{
	this->tiletype = tiletype;
	this->tileVersion = 0;

	this->sprite.setTexture(texture);
	this->sprthlpr.setFrameSize(sf::IntRect(0, 0, 64, 64));

	for (auto sprinf : sprinfs)
	{
		this->sprthlpr.initVersion(sprinf);
	}
}

Tile::Tile()
{}