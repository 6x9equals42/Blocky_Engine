#pragma once

#include <SFML/Graphics.hpp>

#include "sprites.hpp"

enum class EntityType { BUCKET, TORCH, ROCK, BOAT };

class Entity
{
public:
	SpriteHelper sprthlpr;
	sf::Sprite sprite;
	EntityType entityType;

	int entityVersion;

	void draw(sf::RenderWindow& window, float dt);

	void update();

	Entity();

	Entity(sf::Texture& texture, const std::vector<SpriteInfo>& sprinfs, const EntityType entityType);

};