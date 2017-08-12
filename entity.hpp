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
	int pos;

	int entityVersion;

	void draw(sf::RenderWindow& window, float dt);

	void update();

	bool operator==(const Entity& that);

	Entity();

	Entity(sf::Texture& texture, const std::vector<SpriteInfo>& sprinfs, const EntityType entityType);

};