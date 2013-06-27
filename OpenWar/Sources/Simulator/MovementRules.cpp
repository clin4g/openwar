// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "MovementRules.h"
#include "BattleModel.h"



Formation::Formation() :
rankDistance(0),
fileDistance(0),
numberOfRanks(0),
numberOfFiles(0),
_direction(0)
{
}


glm::vec2 Formation::GetFrontLeft(glm::vec2 center)
{
	return center - towardRight * (float)numberOfFiles / 2.0f - towardBack * (float)numberOfRanks / 2.0f;
}


void Formation::SetDirection(float direction)
{
	_direction = direction;
	towardRight = glm::vec2((float)sin(direction), -(float)cos(direction)) * fileDistance;
	towardBack = glm::vec2(-(float)cos(direction), -(float)sin(direction)) * rankDistance;
}


static int GetMaxNumberOfRanks(Unit* unit)
{
	switch (unit->stats.unitPlatform)
	{
		case UnitPlatformGen:
		case UnitPlatformCav:
			return 4;

		default:
			return 4;
	}
}


static bool IsForwardMotion(const std::vector<glm::vec2>& path, glm::vec2 position)
{
	if (path.size() < 2)
		return true;

	glm::vec2 last = *(path.end() - 1);
	glm::vec2 prev = *(path.end() - 2);
	glm::vec2 next = last + (last - prev);

	return glm::length(position - next) < glm::length(position - prev);
}


/*static float path_length(const std::vector<vector2>& path)
{
	float result = 0;
	for (int i = 1; i < (int)path.size(); ++i)
		result += path[i - 1].distance_to(path[i]);
	return result;
}*/



void MovementRules::UpdateMovementPath(std::vector<glm::vec2>& path, glm::vec2 position, glm::vec2 destination, float velocity)
{
	float spacing = 10; //fminf(25, 2.5f + velocity / 15.0f); // length of each segment
	float leading = 10; //fminf(15, spacing + velocity / 10.0f); // length of first segment

	if (path.size() == 0)
		path.push_back(position);

	if (path.size() >= 2)
	{
		glm::vec2 veryLast = *(path.end() - 1);
		glm::vec2 nextLast = *(path.end() - 2);
		if (glm::length(veryLast - nextLast) <= leading)
		{
			path.pop_back();
		}
	}

	while (!IsForwardMotion(path, destination))
		path.pop_back();

	if (path.size() != 0 && glm::length(destination - *(path.end() - 1)) <= leading)
	{
		path.pop_back();
	}

	int n = 20;
	glm::vec2 p = path.size() != 0 ? *(path.end() - 1) : position;
	while (n-- != 0 && glm::length(p - destination) > leading)
	{
		p += spacing * glm::normalize(destination - p);
		path.push_back(p);
	}

	path.push_back(destination);
}


void MovementRules::AdvanceTime(Unit* unit, float timeStep)
{
	while (unit->movement.path.size() != 0 && glm::length(unit->state.center - unit->movement.path[0]) <= 10)
	{
		if (unit->movement.path.size() > 1)
		{
			glm::vec2 p1 = unit->movement.path[0];
			glm::vec2 p2 = unit->movement.path[1];
			unit->movement.path_t0 += glm::length(p1 - p2);
		}
		unit->movement.path.erase(unit->movement.path.begin());
	}

	if (unit->movement.path.size() != 0)
	{
		if (unit->movement.target)
		{
			UpdateMovementPath(unit->movement.path, unit->state.center, unit->movement.target->state.center, 200);
		}

		unit->movement.destination = unit->movement.path[0];
	}
	else if (unit->movement.target)
	{
		unit->movement.destination = unit->movement.target->state.center;
	}

	float count = unit->fightersCount;
	float ranks = unit->formation.numberOfRanks;

	unit->formation.numberOfRanks = (int)fminf(GetMaxNumberOfRanks(unit), count);
	unit->formation.numberOfFiles = (int)ceilf(count / ranks);

	glm::vec2 diff = unit->movement.destination - unit->state.center;
	float direction = glm::length(diff) < 5 ? unit->movement.direction : angle(diff);

	if (unit->movement.target && glm::length(unit->state.center - unit->movement.target->state.center) <= 15)
		direction = angle(unit->movement.target->state.center - unit->state.center);

	if (fabsf(direction - unit->formation._direction) > 0.1f)
	{
		unit->timeUntilSwapFighters = 0;
	}

	unit->formation.SetDirection(direction);

	if (unit->timeUntilSwapFighters < timeStep)
	{
		SwapFighters(unit);
		unit->timeUntilSwapFighters = 5;
	}
	else
	{
		unit->timeUntilSwapFighters -= timeStep;
	}
}


struct FighterPos
{
	Fighter* fighter;
	FighterState state;
	glm::vec2 pos;
};


static bool SortLeftToRight(const FighterPos& v1, const FighterPos& v2) { return v1.pos.y > v2.pos.y; }
static bool SortFrontToBack(const FighterPos& v1, const FighterPos& v2) { return v1.pos.x > v2.pos.x; }


void MovementRules::SwapFighters(Unit* unit)
{
	std::vector<FighterPos> fighters;

	float direction = unit->formation._direction;

	Fighter* fightersEnd = unit->fighters + unit->fightersCount;

	for (Fighter* fighter = unit->fighters; fighter != fightersEnd; ++fighter)
	{
		FighterPos fighterPos;
		fighterPos.fighter = fighter;
		fighterPos.state = fighter->state;
		fighterPos.pos = rotate(fighter->state.position, -direction);
		fighters.push_back(fighterPos);
	}

	std::sort(fighters.begin(), fighters.end(), SortLeftToRight);

	int index = 0;
	while (index < unit->fightersCount)
	{
		int count = unit->fightersCount - index;
		if (count > unit->formation.numberOfRanks)
			count = unit->formation.numberOfRanks;

		std::vector<FighterPos>::iterator begin = fighters.begin() + index;
		std::sort(begin, begin + count, SortFrontToBack);
		while (count-- != 0)
		{
			unit->fighters[index].state = fighters[index].state;
			++index;
		}
	}
}


glm::vec2 MovementRules::NextFighterDestination(Fighter* fighter)
{
	Unit* unit = fighter->unit;

	if (unit->state.IsRouting())
	{
		if (unit->player == Player1)
			return glm::vec2(fighter->state.position.x * 3, -2000);
		else
			return glm::vec2(fighter->state.position.x * 3, 2000);
	}

	int rank = Unit::GetFighterRank(fighter);
	int file = Unit::GetFighterFile(fighter);
	glm::vec2 destination;
	if (rank == 0)
	{
		if (unit->state.unitMode == UnitModeMoving)
		{
			destination = fighter->state.position;
			int n = 1;
			for (int i = 1; i <= 10; ++i)
			{
				Fighter* other = Unit::GetFighter(unit, rank, file - i);
				if (other == 0)
					break;
				destination = (destination + other->state.position + (float)i * unit->formation.towardRight); // / 2;
				++n;
			}
			for (int i = 1; i <= 10; ++i)
			{
				Fighter* other = Unit::GetFighter(unit, rank, file + i);
				if (other == 0)
					break;
				destination = (destination + other->state.position - (float)i * unit->formation.towardRight); // / 2;
				++n;
			}
			destination /= n;
			destination -= glm::normalize(unit->formation.towardBack) * unit->GetSpeed();
		}
		else if (unit->state.unitMode == UnitModeTurning)
		{
			glm::vec2 frontLeft = unit->formation.GetFrontLeft(unit->state.center);
			destination = frontLeft + unit->formation.towardRight * (float)file;
		}
		else
		{
			glm::vec2 frontLeft = unit->formation.GetFrontLeft(unit->movement.destination);
			destination = frontLeft + unit->formation.towardRight * (float)file;
		}
	}
	else
	{
		Fighter* fighterLeft = Unit::GetFighter(unit, rank - 1, file - 1);
		Fighter* fighterMiddle = Unit::GetFighter(unit, rank - 1, file);
		Fighter* fighterRight = Unit::GetFighter(unit, rank - 1, file + 1);

		if (fighterLeft == 0 || fighterRight == 0)
		{
			destination = fighterMiddle->state.destination;
		}
		else
		{
			destination = (fighterLeft->state.destination + fighterRight->state.destination) / 2.0f;
		}
		destination += unit->formation.towardBack;
	}

	return destination;
}
