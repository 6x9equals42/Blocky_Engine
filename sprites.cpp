#include <SFML/Graphics.hpp>

#include "sprites.hpp"

void SpriteHelper::setFrameSize(const sf::IntRect& frameSize)
{
	this->frameSize = frameSize;
}

void SpriteHelper::initVersion(SpriteInfo& ver)
{
	// simply add the new spriteinfo ref to the end of the versions vector
	this->versions.push_back(ver);
}

void SpriteHelper::changeVersion(int version)
{
	// don't change if the change does nothing or isn't valid
	if (this->version == version || version >= int(this->versions.size()) || version < 0) return;

	this->version = version;
	
	sf::IntRect newBounds = this->frameSize;
	newBounds.top = this->versions[version].location.y;
	newBounds.left = this->versions[version].location.x;
	this->bounds = newBounds;

	this->t = 0.f;

	return;
}

sf::IntRect SpriteHelper::getBounds()
{
	return this->bounds;
}

void SpriteHelper::update(const float dt)
{
	if (version >= int(this->versions.size()) || version < 0) return;

	if (this->versions[version].animated)
	{
		float duration = this->versions[version].frameDuration;
		unsigned int numFrames = this->versions[version].numFrames;

		/* if the animation has progressed */
		if (int((t + dt) / duration) > int(t / duration))
		{
			int currentFrame = int((t + dt) / duration);

			currentFrame %= numFrames;

			// create a rect object to describe where the bounds should be now
			sf::IntRect newBounds = this->frameSize; // to get the correct width + height
			newBounds.top = this->versions[version].location.y;
			newBounds.left = this->versions[version].location.x + newBounds.width * currentFrame;

			// and assign the new rectangle to bounds
			this->bounds = newBounds;
		}
		// add dt to t, and adjust for wrap-around
		this->t += dt;
		while (this->t > duration * numFrames)
		{
			this->t -= duration * numFrames;
		}
	}
	


	return;
}

SpriteHelper::SpriteHelper(const sf::IntRect& frameSize)
{
	this->frameSize = frameSize;
	this->t = 0.f;
	this->version = -1;
}

SpriteHelper::SpriteHelper()
{
	this->frameSize = sf::IntRect(0, 0, 1, 1);
	this->t = 0.f;
	this->version = -1;
}