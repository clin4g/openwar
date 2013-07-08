// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef MovementRules_h
#define MovementRules_h

struct Fighter;
struct Unit;


struct Formation
{
	float rankDistance;
	float fileDistance;
	int numberOfRanks; // updated by UpdateFormation() and AddUnit()
	int numberOfFiles; // updated by UpdateFormation() and AddUnit()
	float _direction;
	glm::vec2 towardRight;
	glm::vec2 towardBack;

	Formation();
	glm::vec2 GetFrontLeft(glm::vec2 center);
	void SetDirection(float direction);
};


class MovementRules
{
public:
	static void UpdateMovementPath(std::vector<glm::vec2>& path, glm::vec2 startPosition, glm::vec2 endPosition);
	static float Length(const std::vector<glm::vec2>& path);

	static void AdvanceTime(Unit* unit, float timeStep);
	static void SwapFighters(Unit* unit);
	static glm::vec2 NextFighterDestination(Fighter* fighter);
	static glm::vec2 NextWaypoint(Unit* unit);
};


#endif
