#pragma once

#include "state.hpp"
#include "level.hpp"

class StatePlay : public State
{
private:
	sf::View worldView;
	// add a gui view if I ever need one that's different. Dunno if zooming is a thing yet.


	Level level;



public:
	virtual void draw(Game* game, const float dt);
	virtual void update(Game* game, const float dt);
	virtual void handleInput(Game* game);

	virtual void init(Game* game);

	int levelNum;

	StatePlay();
	StatePlay(Level level);
	StatePlay(int levelNum);
};