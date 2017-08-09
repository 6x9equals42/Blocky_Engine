#pragma once

#include "game.hpp"

class State
{
public:
	virtual void draw(Game* game, const float dt) = 0;
	virtual void update(Game* game, const float dt) = 0;
	virtual void handleInput(Game* game) = 0;

	virtual void init(Game* game) = 0;
};