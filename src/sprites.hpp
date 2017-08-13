#pragma once

#include <SFML/Graphics.hpp>

struct SpriteInfo
{
public:
	bool animated;

	unsigned int numFrames;
	float frameDuration;

	// the location of this sprite version on the sprite sheet
	sf::Vector2i location;
	
	SpriteInfo(sf::Vector2i location)
	{
		this->location = location;
		this->animated = false;
		this->numFrames = 1;
		this->frameDuration = 0.f;
	}

	SpriteInfo(sf::Vector2i location, unsigned int numFrames, float frameDuration)
	{
		this->location = location;
		this->animated = true;
		this->numFrames = numFrames;
		this->frameDuration = frameDuration;
	}

};

class SpriteHelper
{
private:

	/* Have a vector of spriteinfos that can switch between*/
	std::vector<SpriteInfo> versions;

	float t;
	int version;
	sf::IntRect bounds;
	sf::IntRect frameSize;

public:
	int getNumVersions();

	void setFrameSize(const sf::IntRect& frameSize);

	void initVersion(SpriteInfo& ver);

	void update(const float dt);

	void changeVersion(int version);

	sf::IntRect getBounds();

	SpriteHelper(const sf::IntRect& frameSize);
	SpriteHelper();

};