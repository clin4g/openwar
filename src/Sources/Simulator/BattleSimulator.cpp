// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleSimulator.h"
#include "SmoothTerrainWater.h"
#include "TerrainSurface.h"
#include "geometry.h"



SimulationListener::~SimulationListener()
{
}


BattleSimulator::BattleSimulator(BattleModel* battleModel) :
_battleModel(battleModel),
_fighterQuadTree(
	battleModel->terrainSurface->GetBounds().min.x,
	battleModel->terrainSurface->GetBounds().min.y,
	battleModel->terrainSurface->GetBounds().max.x,
	battleModel->terrainSurface->GetBounds().max.y),
_weaponQuadTree(
	battleModel->terrainSurface->GetBounds().min.x,
	battleModel->terrainSurface->GetBounds().min.y,
	battleModel->terrainSurface->GetBounds().max.x,
	battleModel->terrainSurface->GetBounds().max.y),
_secondsSinceLastTimeStep(0),
listener(0),
currentPlayer(PlayerNone),
practice(false)
{
}


void BattleSimulator::AdvanceTime(float secondsSinceLastTime)
{
	//if (this != nullptr)
	//	return;

	recentShootings.clear();
	recentCasualties.clear();

	_secondsSinceLastTimeStep += secondsSinceLastTime;
	while (_secondsSinceLastTimeStep >= _battleModel->timeStep)
	{
		SimulateOneTimeStep();
		_secondsSinceLastTimeStep -= _battleModel->timeStep;
	}

	if (listener != 0)
	{
		for (const Shooting& shooting : recentShootings)
			listener->OnShooting(shooting);

		for (const Casualty casualty : recentCasualties)
			listener->OnCasualty(casualty);
	}

	if (_battleModel->winner == PlayerNone)
	{
		int count1 = 0;
		int count2 = 0;

		for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
		{
			Unit* unit = (*i).second;
			if (!unit->state.IsRouting())
			{
				switch (unit->player)
				{
					case Player1:
						++count1;
						break;
					case Player2:
						++count2;
						break;
					default:
						break;
				}
			}
		}

		if (count1 == 0)
			_battleModel->winner = Player2;
		else if (count2 == 0)
			_battleModel->winner = Player1;
	}
}


void BattleSimulator::SimulateOneTimeStep()
{
	RebuildQuadTree();

	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* unit = (*i).second;
		MovementRules::AdvanceTime(unit, _battleModel->timeStep);
	}

	ComputeNextState();
	AssignNextState();

	ResolveMeleeCombat();
	ResolveMissileCombat();
	RemoveCasualties();
	RemoveDeadUnits();

	_battleModel->time += _battleModel->timeStep;
}


void BattleSimulator::RebuildQuadTree()
{
	_fighterQuadTree.clear();
	_weaponQuadTree.clear();

	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* unit = (*i).second;
		if (unit->state.unitMode != UnitModeInitializing)
		{
			for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
			{
				_fighterQuadTree.insert(fighter->state.position.x, fighter->state.position.y, fighter);

				if (unit->stats.weaponReach > 0)
				{
					glm::vec2 d = unit->stats.weaponReach * vector2_from_angle(fighter->state.direction);
					glm::vec2 p = fighter->state.position + d;
					_weaponQuadTree.insert(p.x, p.y, fighter);
				}
			}
		}
	}
}


void BattleSimulator::ComputeNextState()
{
	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* unit = (*i).second;
		unit->nextState = NextUnitState(unit);

		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
			fighter->nextState = NextFighterState(fighter);
	}
}


void BattleSimulator::AssignNextState()
{
	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* unit = (*i).second;
		unit->state = unit->nextState;

		if (unit->state.IsRouting())
		{
			unit->command.ClearPathAndSetDestination(unit->state.center);
			unit->command.meleeTarget = nullptr;
		}

		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			fighter->state = fighter->nextState;
		}
	}
}


void BattleSimulator::ResolveMeleeCombat()
{
	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* unit = (*i).second;
		bool isMissile = unit->stats.unitWeapon == UnitWeaponArq || unit->stats.unitWeapon == UnitWeaponBow;
		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			Fighter* meleeTarget = fighter->state.meleeTarget;
			if (meleeTarget != nullptr)
			{
				Unit* enemyUnit = meleeTarget->unit;
				float killProbability = 0.5f;

				killProbability *= 1.25f + unit->stats.trainingLevel;
				killProbability *= 1.25f - enemyUnit->stats.trainingLevel;

				if (isMissile)
					killProbability *= 0.15;

				float speed = glm::length(fighter->state.velocity);
				killProbability *= (0.9f + speed / 10.0f);

				float roll = (rand() & 0x7FFF) / (float)0x7FFF;

				if (roll < killProbability)
				{
					meleeTarget->casualty = true;
				}
				else
				{
					meleeTarget->state.readyState = ReadyStateStunned;
					meleeTarget->state.stunnedTimer = 0.6f;
				}

				fighter->state.readyingTimer = fighter->unit->stats.readyingDuration;
			}
		}
	}
}


void BattleSimulator::ResolveMissileCombat()
{
	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* unit = (*i).second;
		bool controlsUnit = practice || currentPlayer == PlayerNone || unit->player == currentPlayer;
		if (controlsUnit && unit->state.shootingCounter > unit->shootingCounter)
		{
			TriggerShooting(unit);
			unit->shootingCounter = unit->state.shootingCounter;
		}
	}

	ResolveProjectileCasualties();
}


void BattleSimulator::TriggerShooting(Unit* unit)
{
	if (unit->state.IsRouting())
		return;

	Shooting shooting;
	shooting.unitWeapon = unit->stats.unitWeapon;

	bool arq = shooting.unitWeapon == UnitWeaponArq;
	float distance = 0;

	for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
	{
		if (fighter->state.readyState == ReadyStatePrepared)
		{
			Projectile projectile;
			projectile.position1 = fighter->state.position;
			projectile.position2 = CalculateFighterMissileTarget(fighter);
			projectile.delay = (arq ? 0.5f : 0.2f) * ((rand() & 0x7FFF) / (float)0x7FFF);
			shooting.projectiles.push_back(projectile);
			distance += glm::length(projectile.position1 - projectile.position2) / unit->fightersCount;
		}
	}

	float speed = arq ? 750 : 75; // meters per second
	shooting.timeToImpact = distance / speed;

	_battleModel->shootings.push_back(shooting);
	recentShootings.push_back(shooting);
}


void BattleSimulator::ResolveProjectileCasualties()
{
	for (std::vector<Shooting>::iterator s = _battleModel->shootings.begin(); s != _battleModel->shootings.end(); ++s)
	{
		Shooting& shooting = *s;

		shooting.timeToImpact -= _battleModel->timeStep;

		std::vector<Projectile>::iterator i = shooting.projectiles.begin();
		while (i != shooting.projectiles.end())
		{
			Projectile& projectile = *i;
			if (shooting.timeToImpact <= 0)
			{
				glm::vec2 hitpoint = projectile.position2;
				for (quadtree<Fighter*>::iterator j(_fighterQuadTree.find(hitpoint.x, hitpoint.y, 0.5f)); *j; ++j)
				{
					Fighter* fighter = **j;
					fighter->casualty = true;
				}
				shooting.projectiles.erase(i);
			}
			else
			{
				++i;
			}
		}
	}
}


void BattleSimulator::RemoveCasualties()
{
	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* unit = (*i).second;
		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			if (fighter->state.opponent != nullptr && fighter->state.opponent->casualty)
				fighter->state.opponent = nullptr;
		}
	}

	bounds2f bounds = _battleModel->terrainSurface->GetBounds();
	glm::vec2 center = bounds.center();
	float radius = bounds.width() / 2;
	float radius_squared = radius * radius;


	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* unit = (*i).second;
		int index = 0;
		int n = unit->fightersCount;
		for (int j = 0; j < n; ++j)
		{
			if (unit->fighters[j].terrainWater && unit->state.IsRouting())
				unit->fighters[j].casualty = true;

			if (unit->fighters[j].casualty)
			{
				++unit->state.recentCasualties;
				recentCasualties.push_back(Casualty(unit->fighters[j].state.position, unit->player, unit->stats.unitPlatform));
			}
			else
			{
				glm::vec2 diff = unit->fighters[j].state.position - center;
				if (glm::dot(diff, diff) < radius_squared)
				{
					if (index < j)
						unit->fighters[index].state = unit->fighters[j].state;
					unit->fighters[index].casualty = false;
					index++;
				}
			}
		}

		unit->fightersCount = index;
	}
}


void BattleSimulator::RemoveDeadUnits()
{
	std::vector<int> remove;
	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* unit = (*i).second;
		if (unit->fightersCount == 0)
		{
			remove.push_back(unit->unitId);
		}
	}

	for (std::vector<int>::iterator i = remove.begin(); i != remove.end(); ++i)
	{
		int unitIndex = *i;
		_battleModel->units.erase(unitIndex);
	}

	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* unit = (*i).second;

		if (unit->command.missileTarget != nullptr && _battleModel->GetUnit(unit->command.missileTarget->unitId) == 0)
			unit->command.missileTarget = nullptr;

		if (unit->command.meleeTarget != nullptr && _battleModel->GetUnit(unit->command.meleeTarget->unitId) == 0)
			unit->command.meleeTarget = nullptr;
	}
}


UnitState BattleSimulator::NextUnitState(Unit* unit)
{
	UnitState result;

	result.center = unit->CalculateUnitCenter();
	result.direction = NextUnitDirection(unit);
	result.unitMode = NextUnitMode(unit);

	result.shootingCounter = unit->state.shootingCounter;

	if (unit->command.missileTargetLocked)
	{
		if (unit->command.missileTarget != nullptr && !IsWithinLineOfFire(unit, unit->command.missileTarget->state.center))
		{
			unit->command.missileTargetLocked = false;
			unit->command.missileTarget = nullptr;
		}
	}

	if (!unit->command.missileTargetLocked && !unit->command.holdFire)
	{
		unit->command.missileTarget = ClosestEnemyWithinLineOfFire(unit);
	}

	if (unit->state.unitMode != UnitModeStanding || unit->command.missileTarget == nullptr)
	{
		result.loadingTimer = 0;
		result.loadingDuration = 0;
	}
	else if (unit->state.loadingTimer + _battleModel->timeStep < unit->state.loadingDuration)
	{
		result.loadingTimer = unit->state.loadingTimer + _battleModel->timeStep;
		result.loadingDuration = unit->state.loadingDuration;
	}
	else
	{
		if (unit->state.loadingDuration != 0
			&& unit->command.missileTarget != nullptr
			&& IsWithinLineOfFire(unit, unit->command.missileTarget->state.center))
		{
			++result.shootingCounter;
		}

		result.loadingTimer = 0;
		result.loadingDuration = 4 + (rand() % 100) / 200.0f;
	}

	result.morale = unit->state.morale;
	if (unit->state.recentCasualties > 0)
	{
		result.morale -= unit->state.recentCasualties * (2.4f - unit->stats.trainingLevel) / 100;
	}
	else if (-0.2f < result.morale && result.morale < 1)
	{
		result.morale += (0.1f + unit->stats.trainingLevel) / 2000;
	}

	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* other = (*i).second;
		float distance = glm::length(other->state.center - unit->state.center);
		float weight = 1.0f * 50.0f / (distance + 50.0f);
		if (other->player == unit->player)
		{
			result.influence -= weight
					* (1 - other->state.morale)
					* (1 - unit->stats.trainingLevel)
					* other->stats.trainingLevel;
		}
	}

	if (_battleModel->winner != PlayerNone && unit->player != _battleModel->winner)
	{
		result.morale = -1;
	}

	if (unit->fightersCount <= 8)
	{
		result.morale = -1;
	}

	if (practice && unit->player == Player2 && unit->state.IsRouting())
	{
		result.morale = -1;
	}

	result.waypoint = MovementRules::NextWaypoint(unit);

	return result;
}


Unit* BattleSimulator::ClosestEnemyWithinLineOfFire(Unit* unit)
{
	Unit* closestEnemy = 0;
	float closestDistance = 10000;
	for (std::map<int, Unit*>::iterator i = _battleModel->units.begin(); i != _battleModel->units.end(); ++i)
	{
		Unit* target = (*i).second;
		if (target->player != unit->player && IsWithinLineOfFire(unit, target->state.center))
		{
			float distance = glm::length(target->state.center - unit->state.center);
			if (distance < closestDistance)
			{
				closestEnemy = target;
				closestDistance = distance;
			}
		}
	}
	return closestEnemy;
}


bool BattleSimulator::IsWithinLineOfFire(Unit* unit, glm::vec2 position)
{
	float distance = glm::length(unit->state.center - position);
	if (distance < 15 || distance > unit->stats.maximumRange)
		return false;

	float a = diff_radians(unit->state.direction, angle(position - unit->state.center));
	if (fabsf(a) > M_PI_4)
		return false;

	return true;
}


UnitMode BattleSimulator::NextUnitMode(Unit* unit)
{
	switch (unit->state.unitMode)
	{
		case UnitModeInitializing:
			return UnitModeStanding;

		case UnitModeStanding:
			if (glm::length(unit->state.center - unit->command.GetDestination()) > 8)
				return UnitModeMoving;
			break;

		case UnitModeMoving:
			if (glm::length(unit->state.center - unit->command.GetDestination()) <= 8)
				return UnitModeStanding;
			break;

		default:
			break;
	}
	return unit->state.unitMode;
}


float BattleSimulator::NextUnitDirection(Unit* unit)
{
	if (true) // unit->movement
		return unit->command.facing;
	else
		return unit->state.direction;
}


FighterState BattleSimulator::NextFighterState(Fighter* fighter)
{
	const FighterState& original = fighter->state;
	FighterState result;

	result.readyState = original.readyState;
	result.position = NextFighterPosition(fighter);
	result.velocity = NextFighterVelocity(fighter);


	// DIRECTION

	if (fighter->unit->state.unitMode == UnitModeMoving)
	{
		result.direction = angle(original.velocity);
	}
	else if (original.opponent != nullptr)
	{
		result.direction = angle(original.opponent->state.position - original.position);
	}
	else
	{
		result.direction = fighter->unit->state.direction;
	}


	// OPPONENT

	if (original.opponent != 0 && glm::length(original.position - original.opponent->state.position) <= fighter->unit->stats.weaponReach * 2)
	{
		result.opponent = original.opponent;
	}
	else if (fighter->unit->state.unitMode != UnitModeMoving && !fighter->unit->state.IsRouting())
	{
		result.opponent = FindFighterStrikingTarget(fighter);
	}

	// DESTINATION

	if (original.opponent != nullptr)
	{
		result.destination = original.opponent->state.position
				- fighter->unit->stats.weaponReach * vector2_from_angle(original.direction);
	}
	else
	{
		switch (original.readyState)
		{
			case ReadyStateUnready:
			case ReadyStateReadying:
			case ReadyStatePrepared:
				result.destination = MovementRules::NextFighterDestination(fighter);
				break;

			default:
				result.destination = original.position;
				break;
		}
	}

	// READY STATE

	switch (original.readyState)
	{
		case ReadyStateUnready:
			if (fighter->unit->command.meleeTarget != nullptr)
			{
				result.readyState = ReadyStatePrepared;
			}
			else if (fighter->unit->state.unitMode == UnitModeStanding)
			{
				result.readyState = ReadyStateReadying;
				result.readyingTimer = fighter->unit->stats.readyingDuration;
			}
			break;

		case ReadyStateReadying:
			if (original.readyingTimer > _battleModel->timeStep)
			{
				result.readyingTimer = original.readyingTimer - _battleModel->timeStep;
			}
			else
			{
				result.readyingTimer = 0;
				result.readyState = ReadyStatePrepared;
			}
			break;

		case ReadyStatePrepared:
			if (fighter->unit->state.unitMode == UnitModeMoving && fighter->unit->command.meleeTarget == nullptr)
			{
				result.readyState = ReadyStateUnready;
			}
			else if (result.opponent != nullptr)
			{
				result.readyState = ReadyStateStriking;
				result.strikingTimer = fighter->unit->stats.strikingDuration;
			}
			break;

		case ReadyStateStriking:
			if (original.strikingTimer > _battleModel->timeStep)
			{
				result.strikingTimer = original.strikingTimer - _battleModel->timeStep;
				result.opponent = original.opponent;
			}
			else
			{
				result.meleeTarget = original.opponent;
				result.strikingTimer = 0;
				result.readyState = ReadyStateReadying;
				result.readyingTimer = fighter->unit->stats.readyingDuration;
			}
			break;

		case ReadyStateStunned:
			if (original.stunnedTimer > _battleModel->timeStep)
			{
				result.stunnedTimer = original.stunnedTimer - _battleModel->timeStep;
			}
			else
			{
				result.stunnedTimer = 0;
				result.readyState = ReadyStateReadying;
				result.readyingTimer = fighter->unit->stats.readyingDuration;
			}
			break;
	}

	return result;
}


glm::vec2 BattleSimulator::NextFighterPosition(Fighter* fighter)
{
	Unit* unit = fighter->unit;

	if (unit->state.unitMode == UnitModeInitializing)
	{
		glm::vec2 center = unit->state.center;
		glm::vec2 frontLeft = unit->formation.GetFrontLeft(center);
		glm::vec2 offsetRight = unit->formation.towardRight * (float)Unit::GetFighterFile(fighter);
		glm::vec2 offsetBack = unit->formation.towardBack * (float)Unit::GetFighterRank(fighter);
		return frontLeft + offsetRight + offsetBack;
	}
	else
	{
		glm::vec2 result = fighter->state.position + fighter->state.velocity * _battleModel->timeStep;
		glm::vec2 adjust;
		int count = 0;

		const float fighterDistance = 0.9f;

		for (quadtree<Fighter*>::iterator i(_fighterQuadTree.find(result.x, result.y, fighterDistance)); *i; ++i)
		{
			Fighter* obstacle = **i;
			if (obstacle != fighter)
			{
				glm::vec2 position = obstacle->state.position;
				glm::vec2 diff = position - result;
				if (glm::dot(diff, diff) < fighterDistance * fighterDistance)
				{
					adjust -= glm::normalize(diff) * fighterDistance;
					++count;
				}
			}
		}

		const float weaponDistance = 0.75f;

		for (quadtree<Fighter*>::iterator i(_weaponQuadTree.find(result.x, result.y, weaponDistance)); *i; ++i)
		{
			Fighter* obstacle = **i;
			if (obstacle->unit->player != unit->player)
			{
				glm::vec2 r = obstacle->unit->stats.weaponReach * vector2_from_angle(obstacle->state.direction);
				glm::vec2 position = obstacle->state.position + r;
				glm::vec2 diff = position - result;
				if (glm::dot(diff, diff) < weaponDistance * weaponDistance)
				{
					diff = obstacle->state.position - result;
					adjust -= glm::normalize(diff) * weaponDistance;
					++count;
				}
			}
		}

		if (count != 0)
		{
			result += adjust / (float)count;
		}

		return result;
	}
}


glm::vec2 BattleSimulator::NextFighterVelocity(Fighter* fighter)
{
	Unit* unit = fighter->unit;
	float speed = unit->GetSpeed();

	switch (fighter->state.readyState)
	{
		case ReadyStateStriking:
			speed = unit->stats.walkingSpeed / 4;
			break;

		case ReadyStateStunned:
			speed = unit->stats.walkingSpeed / 4;
			break;

		default:
			break;
	}

	if (glm::length(fighter->state.position - fighter->terrainPosition) > 5)
	{
		fighter->terrainPosition = fighter->state.position;
		fighter->terrainForest = _battleModel->terrainSurface->IsForest(fighter->state.position);
		fighter->terrainWater = _battleModel->terrainWater->IsWater(fighter->state.position);
	}

	if (fighter->terrainForest)
	{
		if (unit->stats.unitPlatform == UnitPlatformCav || unit->stats.unitPlatform == UnitPlatformGen)
			speed *= 0.5;
		else
			speed *= 0.9;
	}

	glm::vec2 diff = fighter->state.destination - fighter->state.position;
	float diff_len = glm::dot(diff, diff);
	if (diff_len < 0.01)
		return diff;

	glm::vec2 delta = glm::normalize(diff) * speed;
	float delta_len = glm::dot(delta, delta);

	return delta_len < diff_len ? delta : diff;
}


Fighter* BattleSimulator::FindFighterStrikingTarget(Fighter* fighter)
{
	Unit* unit = fighter->unit;

	glm::vec2 position = fighter->state.position + unit->stats.weaponReach * vector2_from_angle(fighter->state.direction);
	float radius = 1.1f;

	for (quadtree<Fighter*>::iterator i(_fighterQuadTree.find(position.x, position.y, radius)); *i; ++i)
	{
		Fighter* target = **i;
		if (target != fighter && target->unit->player != unit->player)
		{
			return target;
		}
	}

	return 0;
}


glm::vec2 BattleSimulator::CalculateFighterMissileTarget(Fighter* fighter)
{
	Unit* unit = fighter->unit;

	if (unit->command.missileTarget != nullptr)
	{
		float dx = 10.0f * ((rand() & 255) / 128.0f - 1.0f);
		float dy = 10.0f * ((rand() & 255) / 127.0f - 1.0f);
		return unit->command.missileTarget->state.center + glm::vec2(dx, dy);
	}

	return glm::vec2();
}
