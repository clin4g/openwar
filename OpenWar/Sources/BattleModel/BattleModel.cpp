// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleModel.h"
#include "BattleView.h"

#include "ShootingCounter.h"
#include "SmokeCounter.h"
#include "UnitCounter.h"



Movement::Movement() :
path(),
path_t0(0),
destination(),
direction(0),
target(0),
running(false)
{
}


Projectile::Projectile() :
position1(),
position2(),
delay(0)
{
}


Shooting::Shooting() :
unitWeapon(UnitWeaponBow),
timeToImpact(0),
projectiles()
{
}


FighterState::FighterState() :
readyState(ReadyStateUnready),
destination(),
position(),
velocity(),
direction(0),
opponent(0),
meleeTarget(0),
readyingTimer(0),
strikingTimer(0),
stunnedTimer(0)
{
}


Fighter::Fighter() :
unit(0),
state(),
nextState(),
casualty(false),
terrainForest(false),
terrainWater(false),
terrainPosition()
{
}


FighterUpdate Fighter::GetFighterUpdate(const UnitUpdate& unitUpdate)
{
	FighterUpdate result;

	result.positionX = (unsigned char)(255.0f * (state.position.x - unitUpdate.minX) / (unitUpdate.maxX - unitUpdate.minX));
	result.positionY = (unsigned char)(255.0f * (state.position.y - unitUpdate.minY) / (unitUpdate.maxY - unitUpdate.minY));

	return result;
}


void Fighter::SetFighterUpdate(const UnitUpdate& unitUpdate, const FighterUpdate& fighterUpdate)
{
	float positionX = unitUpdate.minX + (unitUpdate.maxX - unitUpdate.minX) * (float)fighterUpdate.positionX / 255.0f;
	float positionY = unitUpdate.minY + (unitUpdate.maxY - unitUpdate.minY) * (float)fighterUpdate.positionY / 255.0f;

	if (positionX == 0)
		state.position = glm::vec2(positionX, positionY);
	else
		state.position = glm::vec2(positionX, positionY);
}


UnitState::UnitState() :
unitMode(UnitModeInitializing),
center(),
direction(0),
shootingTimer(0),
shootingCounter(0),
recentCasualties(0),
morale(1),
influence(0)
{
}


UnitUpdate::UnitUpdate()
{
}


UnitUpdate Unit::GetUnitUpdate()
{
	UnitUpdate result;

	result.unitId = unitId;
	result.fightersCount = fightersCount;
	result.morale = state.morale;

	result.movementDestination = movement.destination;
	result.movementDirection = movement.direction;
	result.movementTargetUnitId = movement.target != 0 ? movement.target->unitId : 0;
	result.movementRunning = movement.running;

	result.minX = 20000;
	result.maxX = -20000;
	result.minY = 20000;
	result.maxY = -20000;

	for (int i = 0; i < fightersCount; ++i)
	{
		glm::vec2 p = fighters[i].state.position;
		result.minX = fminf(result.minX, p.x);
		result.maxX = fmaxf(result.maxX, p.x);
		result.minY = fminf(result.minY, p.y);
		result.maxY = fmaxf(result.maxY, p.y);
	}

	return result;
}


void Unit::SetUnitUpdate(UnitUpdate unitUpdate, BattleModel* battleModel)
{
	fightersCount = unitUpdate.fightersCount;

	state.morale = unitUpdate.morale;

	movement.destination = unitUpdate.movementDestination;
	movement.direction = unitUpdate.movementDirection;
	movement.target = battleModel->GetUnit(unitUpdate.movementTargetUnitId);
	movement.running = unitUpdate.movementRunning;
}


UnitStats::UnitStats() :
unitPlatform(UnitPlatformCav),
unitWeapon(UnitWeaponYari),
weaponReach(0),
trainingLevel(0),
strikingDuration(0),
fireRate(25.0f), // rounds per minute
minimumRange(0),
maximumRange(0),
fireAccuracy(0.7f),
walkingSpeed(0),
runningSpeed(0),
fighterSize(glm::vec2(10, 10)),
spacing(glm::vec2(10, 10))
{
}


Unit::Unit() :
player(Player1),
stats(),
state(),
nextState(),
formation(),
movement(),
fighters(0),
fightersCount(0),
timeUntilSwapFighters(0),
missileTarget(0),
missileTargetLocked(false),
shootingCounter(0)
{
}


glm::vec2 Unit::CalculateUnitCenter()
{
	if (state.unitMode == UnitModeInitializing)
		return state.center;

	glm::vec2 p = glm::vec2();
	int count = 0;

	for (Fighter* fighter = fighters, * end = fighter + fightersCount; fighter != end; ++fighter)
	{
		p += fighter->state.position;
		++count;
	}

	return p / (float)count;
}


float Unit::GetSpeed()
{
	if (state.IsRouting())
		return stats.runningSpeed * 1.2f;
	else
		return movement.running || movement.target ? stats.runningSpeed : stats.walkingSpeed;
}


int Unit::GetFighterRank(Fighter* fighter)
{
	Unit* unit = fighter->unit;
	return (fighter - unit->fighters) % unit->formation.numberOfRanks;
}


int Unit::GetFighterFile(Fighter* fighter)
{
	Unit* unit = fighter->unit;
	return (int)(fighter - unit->fighters) / unit->formation.numberOfRanks;
}


Fighter* Unit::GetFighter(Unit* unit, int rank, int file)
{
	if (0 <= rank && rank < unit->formation.numberOfRanks && file >= 0)
	{
		int index = rank + file * unit->formation.numberOfRanks;
		if (index < unit->fightersCount)
			return unit->fighters + index;
	}
	return 0;
}



BattleModel::BattleModel() :
lastUnitId(0),
winner(PlayerNone),
time(0),
timeStep(1.0f / 15.0f),
_mapSize(1024, 1024),
_unitMarkers()
{
}


BattleModel::~BattleModel()
{
	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		delete[] unit->fighters;
		delete unit;
	}

	for (ShootingCounter* marker : _shootingMarkers)
		delete marker;

	for (SmokeCounter* marker : _smokeMarkers)
		delete marker;

	for (UnitCounter* marker : _unitMarkers)
		delete marker;
}



bool BattleModel::IsMelee() const
{
	for (std::map<int, Unit*>::const_iterator i = units.begin(); i != units.end(); ++i)
	{
		const Unit* unit = (*i).second;
		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			if (fighter->state.opponent)
			{
				return true;
			}
		}
	}
	return false;
}


Unit* BattleModel::AddUnit(Player player, int numberOfFighters, UnitStats stats, glm::vec2 position)
{
	Unit* unit = new Unit();

	unit->unitId = ++lastUnitId;
	unit->player = player;
	unit->stats = stats;

	unit->fightersCount = numberOfFighters;
	unit->fighters = new Fighter[numberOfFighters];

	for (Fighter* i = unit->fighters, * end = i + numberOfFighters; i != end; ++i)
		i->unit = unit;

	unit->movement.direction = player == Player1 ? (float)M_PI_2 : (float)M_PI_2 * 3;
	unit->movement.destination = position;

	unit->state.unitMode = UnitModeInitializing;
	unit->state.center = position;
	unit->state.direction = unit->movement.direction;

	unit->missileTarget = 0;

	unit->formation.rankDistance = stats.fighterSize.y + stats.spacing.y;
	unit->formation.fileDistance = stats.fighterSize.x + stats.spacing.x;
	unit->formation.numberOfRanks = (int)fminf(6, unit->fightersCount);
	unit->formation.numberOfFiles = (int)ceilf((float)unit->fightersCount / unit->formation.numberOfRanks);

	units[unit->unitId] = unit;

	return unit;
}


UnitStats BattleModel::GetDefaultUnitStats(UnitPlatform unitPlatform, UnitWeapon unitWeapon)
{
	UnitStats result;
	result.unitPlatform = unitPlatform;
	result.unitWeapon = unitWeapon;

	if (unitPlatform == UnitPlatformCav || unitPlatform == UnitPlatformGen)
	{
		result.fighterSize = glm::vec2(1.1f, 2.3f);
		result.spacing = glm::vec2(1.1f, 1.7f);
		result.walkingSpeed = 7;
		result.runningSpeed = 14;
	}
	else
	{
		result.fighterSize = glm::vec2(0.7f, 0.3f);
		result.spacing = glm::vec2(1.1f, 0.9f);
		result.walkingSpeed = 4;
		result.runningSpeed = 8;
	}

	result.readyingDuration = 1.0f;

	switch (unitWeapon)
	{
		case UnitWeaponBow:
			result.maximumRange = 150;
			result.runningSpeed += 2; // increased speed
			result.strikingDuration = 3.0f;
			break;

		case UnitWeaponArq:
			result.maximumRange = 110;
			result.walkingSpeed = 5;
			result.runningSpeed = 9;
			result.strikingDuration = 3.0f;
			break;

		case UnitWeaponYari:
			result.weaponReach = 5.0f;
			result.strikingDuration = 2.0f;
			break;

		case UnitWeaponNagi:
			result.weaponReach = 2.4f;
			result.strikingDuration = 1.9f;
			break;

		case UnitWeaponKata:
			result.weaponReach = 1.0f;
			result.strikingDuration = 1.8f;
			break;
	}

	switch (result.unitPlatform)
	{
		case UnitPlatformAsh:
			result.trainingLevel = 0.5f;
			break;
		case UnitPlatformGen:
			result.trainingLevel = 0.9f;
			break;
		default:
			result.trainingLevel = 0.8f;
			break;
	}

	return result;
}



template <class T> void AnimateMarkers(std::vector<T*>& markers, float seconds)
{
	size_t index = 0;
	while (index < markers.size())
	{
		T* marker = markers[index];
		if (marker->Animate(seconds))
		{
			++index;
		}
		else
		{
			markers.erase(markers.begin() + index);
			delete marker;
		}
	}
}


void BattleModel::AnimateMarkers(float seconds)
{
	::AnimateMarkers(_unitMarkers, seconds);
	::AnimateMarkers(_shootingMarkers, seconds);
	::AnimateMarkers(_smokeMarkers, seconds);
}


void BattleModel::InitializeUnitMarkers()
{
	for (std::pair<int, Unit*> item : units)
	{
		Unit* unit = item.second;
		AddUnitMarker(unit);
	}
}


void BattleModel::AddUnitMarker(Unit* unit)
{
	UnitCounter* marker = new UnitCounter(this, unit);
	marker->Animate(0);
	_unitMarkers.push_back(marker);
}


void BattleModel::AddShootingAndSmokeMarkers(const Shooting& shooting)
{
	AddShootingMarker(shooting);
	if (shooting.unitWeapon == UnitWeaponArq)
		AddSmokeMarker(shooting);
}


void BattleModel::AddShootingMarker(const Shooting& shooting)
{
	ShootingCounter* shootingMarker = AddShootingMarker(shooting.unitWeapon);

	for (const Projectile& projectile : shooting.projectiles)
	{
		glm::vec3 p1 = glm::vec3(projectile.position1, terrainSurface->GetHeight(projectile.position1));
		glm::vec3 p2 = glm::vec3(projectile.position2, terrainSurface->GetHeight(projectile.position2));
		shootingMarker->AddProjectile(p1, p2, projectile.delay, shooting.timeToImpact);
	}
}


ShootingCounter* BattleModel::AddShootingMarker(UnitWeapon unitWeapon)
{
	ShootingCounter* marker = new ShootingCounter(unitWeapon);
	_shootingMarkers.push_back(marker);
	return marker;
}


void BattleModel::RemoveAllShootingMarkers()
{
	for (ShootingCounter* marker : _shootingMarkers)
	{
		marker->Animate(100);
	}
}


void BattleModel::AddSmokeMarker(const Shooting& shooting)
{
	SmokeCounter* marker = AddSmokeMarker(shooting.unitWeapon);

	for (const Projectile& projectile : shooting.projectiles)
	{
		glm::vec3 p1 = glm::vec3(projectile.position1, terrainSurface->GetHeight(projectile.position1));
		glm::vec3 p2 = glm::vec3(projectile.position2, terrainSurface->GetHeight(projectile.position2));
		marker->AddParticle(p1, p2, projectile.delay);
	}
}


SmokeCounter* BattleModel::AddSmokeMarker(UnitWeapon unitWeapon)
{
	SmokeCounter* marker = new SmokeCounter(unitWeapon);
	_smokeMarkers.push_back(marker);
	return marker;
}


void BattleModel::RemoveAllSmokeMarkers()
{
	for (SmokeCounter* marker : _smokeMarkers)
	{
		marker->Animate(100);
	}
}


UnitCounter* BattleModel::GetNearestUnitMarker(glm::vec2 position, Player player)
{
	UnitCounter* result = 0;
	float nearest = INFINITY;

	for (UnitCounter* marker : _unitMarkers)
	{
		Unit* unit = marker->_unit;
		if (player != PlayerNone && unit->player != player)
			continue;

		glm::vec2 p = unit->state.center;
		float dx = p.x - position.x;
		float dy = p.y - position.y;
		float d = dx * dx + dy * dy;
		if (d < nearest)
		{
			result = marker;
			nearest = d;
		}
	}

	return result;
}
