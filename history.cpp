#include "history.hpp"

void History::pushLevel(LevelState levelState)
{
	levelStates.push(levelState);
}


LevelState History::popLevel()
{
	if (levelStates.empty())
		return startingPosition;
	levelStates.pop();
	if (levelStates.empty())
		return startingPosition;
	return levelStates.top();
}

LevelState History::getLevel()
{
	if (levelStates.empty())
		return startingPosition;
	return levelStates.top();
}

History::History()
{
}

History::History(LevelState levelState)
{
	this->startingPosition = levelState;
}