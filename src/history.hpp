#pragma once

#include <stack>
#include <vector>

#include "entity.hpp"

struct LevelState
{
public:
	int playerPos;
	
	std::vector<int> tileVersions;
	std::vector<Entity> entities;
	std::vector<Entity> subEntities;
};

class History
{
private:
	std::stack<LevelState> levelStates;
	LevelState startingPosition;

public:

	void pushLevel(LevelState levelState);
	LevelState popLevel();
	LevelState getLevel();

	History();
	History(LevelState levelState);
};