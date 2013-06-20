// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEMODEL_H
#define BATTLEMODEL_H

#include "sprite.h"
#include "vertexbuffer.h"
#include "SimulationState.h"
#include "TerrainSurfaceModelSmooth.h"

class BattleContext;
class BattleModel;
class CasualtyMarker;
class UnitCounter;
class MovementMarker;
class TrackingMarker;
class RangeMarker;
class ShootingCounter;
class SmokeCounter;


class BattleModel
{
public:
	BattleContext* _battleContext;
	glm::vec2 _mapSize;
	Player _player;

	std::vector<UnitCounter*> _unitMarkers;
	std::vector<ShootingCounter*> _shootingMarkers;
	std::vector<SmokeCounter*> _smokeMarkers;

public:
	BattleModel(BattleContext* battleContext);
	~BattleModel();

	BattleContext* GetBattleContext() const { return _battleContext; }

	void AnimateMarkers(float seconds);

	void Initialize(SimulationState* simulationState);

	void AddUnitMarker(Unit* unit);

	void AddShootingAndSmokeMarkers(const Shooting& shooting);

	void AddShootingMarker(const Shooting& shooting);
	ShootingCounter* AddShootingMarker(UnitWeapon unitWeapon);
	void RemoveAllShootingMarkers();

	void AddSmokeMarker(const Shooting& shooting);
	SmokeCounter* AddSmokeMarker(UnitWeapon unitWeapon);
	void RemoveAllSmokeMarkers();

	UnitCounter* GetNearestUnitMarker(glm::vec2 position, Player player);
};



#endif
