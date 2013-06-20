// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleModel.h"
#include "BattleView.h"
#include "BattleContext.h"

#include "CasualtyMarker.h"
#include "MovementMarker.h"
#include "RangeMarker.h"
#include "ShootingMarker.h"
#include "SmokeMarker.h"
#include "TrackingMarker.h"
#include "UnitMarker.h"



BattleModel::BattleModel(BattleContext* battleContext) :
_battleContext(battleContext),
_player(PlayerNone),
_mapSize(1024, 1024),
_unitMarkers(),
_movementMarkers(),
_trackingMarkers(),
_casualtyMarker(0)
{
}


BattleModel::~BattleModel()
{
	delete _casualtyMarker;

	for (RangeMarker* marker : _rangeMarkers)
		delete marker;

	for (ShootingMarker* marker : _shootingMarkers)
		delete marker;

	for (SmokeMarker* marker : _smokeMarkers)
		delete marker;

	for (UnitMarker* marker : _unitMarkers)
		delete marker;

	for (MovementMarker* marker : _movementMarkers)
		delete marker;

	for (TrackingMarker* marker : _trackingMarkers)
		delete marker;
}


void BattleModel::AnimateMarkers(float seconds)
{
	_casualtyMarker->Animate(seconds);
	AnimateMarkers(_unitMarkers, seconds);
	AnimateMarkers(_movementMarkers, seconds);
	AnimateMarkers(_rangeMarkers, seconds);
	AnimateMarkers(_shootingMarkers, seconds);
	AnimateMarkers(_smokeMarkers, seconds);
}


template <class T> void BattleModel::AnimateMarkers(std::vector<T*>& markers, float seconds)
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


void BattleModel::Initialize(SimulationState* simulationState)
{
	_casualtyMarker = new CasualtyMarker();

	for (std::pair<int, Unit*> item : simulationState->units)
	{
		Unit* unit = item.second;
		AddUnitMarker(unit);
		if (unit->stats.maximumRange > 0)
			AddRangeMarker(unit);
	}
}


void BattleModel::AddUnitMarker(Unit* unit)
{
	UnitMarker* marker = new UnitMarker(this, unit);
	marker->Animate(0);
	_unitMarkers.push_back(marker);
}


void BattleModel::AddRangeMarker(Unit* unit)
{
	RangeMarker* marker = new RangeMarker(this, unit);
	marker->Animate(0);
	_rangeMarkers.push_back(marker);
}


void BattleModel::AddCasualty(const Casualty& casualty)
{
	glm::vec3 position = glm::vec3(casualty.position, _battleContext->terrainSurfaceModel->GetHeight(casualty.position));
	_casualtyMarker->AddCasualty(position, casualty.player, casualty.platform);
}


MovementMarker* BattleModel::AddMovementMarker(Unit* unit)
{
	MovementMarker* marker = new MovementMarker(this, unit);
	_movementMarkers.push_back(marker);
	return marker;
}


MovementMarker* BattleModel::GetMovementMarker(Unit* unit)
{
	for (MovementMarker* marker : _movementMarkers)
		if (marker->_unit == unit)
			return marker;

	return 0;
}


TrackingMarker* BattleModel::AddTrackingMarker(Unit* unit)
{
	TrackingMarker* trackingMarker = new TrackingMarker(unit);
	_trackingMarkers.push_back(trackingMarker);
	return trackingMarker;
}


TrackingMarker* BattleModel::GetTrackingMarker(Unit* unit)
{
	for (TrackingMarker* marker : _trackingMarkers)
		if (marker->_unit == unit)
			return marker;

	return 0;
}


void BattleModel::RemoveTrackingMarker(TrackingMarker* trackingMarker)
{
	auto i = std::find(_trackingMarkers.begin(), _trackingMarkers.end(), trackingMarker);
	if (i != _trackingMarkers.end())
	{
		_trackingMarkers.erase(i);
		delete trackingMarker;
	}
}


void BattleModel::AddShootingAndSmokeMarkers(const Shooting& shooting)
{
	AddShootingMarker(shooting);
	if (shooting.unitWeapon == UnitWeaponArq)
		AddSmokeMarker(shooting);
}


void BattleModel::AddShootingMarker(const Shooting& shooting)
{
	ShootingMarker* shootingMarker = AddShootingMarker(shooting.unitWeapon);

	for (const Projectile& projectile : shooting.projectiles)
	{
		glm::vec3 p1 = glm::vec3(projectile.position1, _battleContext->terrainSurfaceModel->GetHeight(projectile.position1));
		glm::vec3 p2 = glm::vec3(projectile.position2, _battleContext->terrainSurfaceModel->GetHeight(projectile.position2));
		shootingMarker->AddProjectile(p1, p2, projectile.delay, shooting.timeToImpact);
	}
}


ShootingMarker* BattleModel::AddShootingMarker(UnitWeapon unitWeapon)
{
	ShootingMarker* marker = new ShootingMarker(unitWeapon);
	_shootingMarkers.push_back(marker);
	return marker;
}


void BattleModel::RemoveAllShootingMarkers()
{
	for (ShootingMarker* marker : _shootingMarkers)
	{
		marker->Animate(100);
	}
}


void BattleModel::AddSmokeMarker(const Shooting& shooting)
{
	SmokeMarker* marker = AddSmokeMarker(shooting.unitWeapon);

	for (const Projectile& projectile : shooting.projectiles)
	{
		glm::vec3 p1 = glm::vec3(projectile.position1, _battleContext->terrainSurfaceModel->GetHeight(projectile.position1));
		glm::vec3 p2 = glm::vec3(projectile.position2, _battleContext->terrainSurfaceModel->GetHeight(projectile.position2));
		marker->AddParticle(p1, p2, projectile.delay);
	}
}


SmokeMarker* BattleModel::AddSmokeMarker(UnitWeapon unitWeapon)
{
	SmokeMarker* marker = new SmokeMarker(unitWeapon);
	_smokeMarkers.push_back(marker);
	return marker;
}


void BattleModel::RemoveAllSmokeMarkers()
{
	for (SmokeMarker* marker : _smokeMarkers)
	{
		marker->Animate(100);
	}
}


UnitMarker* BattleModel::GetNearestUnitMarker(glm::vec2 position, Player player)
{
	UnitMarker* result = 0;
	float nearest = INFINITY;

	for (UnitMarker* marker : _unitMarkers)
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


MovementMarker* BattleModel::GetNearestMovementMarker(glm::vec2 position, Player player)
{
	MovementMarker* result = 0;
	float nearest = INFINITY;

	for (MovementMarker* marker : _movementMarkers)
	{
		Unit* unit = marker->_unit;
		if (player != PlayerNone && unit->player != player)
			continue;

		glm::vec2 p = unit->movement.GetFinalDestination();
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
