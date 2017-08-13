#include <SFML/Graphics.hpp>

#include "player.hpp"

void Player::draw(sf::RenderWindow & window, float dt)
{
	this->sprthlpr.changeVersion(this->playerVersion);

	this->sprthlpr.update(dt);

	this->sprite.setTextureRect(this->sprthlpr.getBounds());

	window.draw(this->sprite);
}

void Player::update()
{
	// not sure if this is ever necessary?
}

Player::Player()
{
}

Player::Player(sf::Texture& texture, const std::vector<SpriteInfo>& sprinfs)
{
	this->playerVersion = 0;


	this->sprite.setTexture(texture);
	this->sprthlpr.setFrameSize(sf::IntRect(0, 0, 64, 64));

	for (auto sprinf : sprinfs)
	{
		this->sprthlpr.initVersion(sprinf);
	}
}