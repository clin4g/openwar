// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleModel.h"
#include "BattleView.h"
#include "BattleContext.h"

#include "CasualtyMarker.h"
#include "MovementMarker.h"
#include "RangeMarker.h"
#include "ShootingCounter.h"
#include "SmokeCounter.h"
#include "TrackingMarker.h"
#include "UnitCounter.h"



BattleModel::BattleModel(BattleContext* battleContext) :
_battleContext(battleContext),
_player(PlayerNone),
_mapSize(1024, 1024),
_unitMarkers()
{
}


BattleModel::~BattleModel()
{
	for (ShootingCounter* marker : _shootingMarkers)
		delete marker;

	for (SmokeCounter* marker : _smokeMarkers)
		delete marker;

	for (UnitCounter* marker : _unitMarkers)
		delete marker;
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


void BattleModel::Initialize(SimulationState* simulationState)
{
	for (std::pair<int, Unit*> item : simulationState->units)
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
		glm::vec3 p1 = glm::vec3(projectile.position1, _battleContext->terrainSurfaceModel->GetHeight(projectile.position1));
		glm::vec3 p2 = glm::vec3(projectile.position2, _battleContext->terrainSurfaceModel->GetHeight(projectile.position2));
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
		glm::vec3 p1 = glm::vec3(projectile.position1, _battleContext->terrainSurfaceModel->GetHeight(projectile.position1));
		glm::vec3 p2 = glm::vec3(projectile.position2, _battleContext->terrainSurfaceModel->GetHeight(projectile.position2));
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
