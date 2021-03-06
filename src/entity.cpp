#include <SFML/Graphics.hpp>

#include "entity.hpp"

void Entity::draw(sf::RenderWindow& window, float dt)
{
	this->sprthlpr.changeVersion(this->entityVersion);

	this->sprthlpr.update(dt);

	this->sprite.setTextureRect(this->sprthlpr.getBounds());

	window.draw(this->sprite);
}

void Entity::update()
{}

bool Entity::operator==(const Entity & that)
{
	return (this->pos == that.pos && this->entityType == that.entityType && this->entityVersion == that.entityVersion);
}

Entity::Entity()
{
	this->entityVersion = -1;
}

Entity::Entity(sf::Texture& texture, const std::vector<SpriteInfo>& sprinfs, const EntityType entityType)
{
	this->entityVersion = 0;
	this->entityType = entityType;

	this->pos = -1;


	this->sprite.setTexture(texture);
	this->sprthlpr.setFrameSize(sf::IntRect(0, 0, 64, 64));

	for (auto sprinf : sprinfs)
	{
		this->sprthlpr.initVersion(sprinf);
	}
}