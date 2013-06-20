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
	CasualtyMarker* _casualtyMarker;
	std::vector<UnitCounter*> _unitMarkers;
	std::vector<MovementMarker*> _movementMarkers;
	std::vector<TrackingMarker*> _trackingMarkers;
	std::vector<RangeMarker*> _rangeMarkers;
	std::vector<ShootingCounter*> _shootingMarkers;
	std::vector<SmokeCounter*> _smokeMarkers;

public:
	BattleModel(BattleContext* battleContext);
	~BattleModel();

	BattleContext* GetBattleContext() const { return _battleContext; }

	void AnimateMarkers(float seconds);

	template <class T> void AnimateMarkers(std::vector<T*>& markers, float seconds);

	void Initialize(SimulationState* simulationState);

	void AddUnitMarker(Unit* unit);
	void AddRangeMarker(Unit* unit);
	void AddCasualty(const Casualty& casualty);

	MovementMarker* AddMovementMarker(Unit* unit);
	MovementMarker* GetMovementMarker(Unit* unit);

	TrackingMarker* AddTrackingMarker(Unit* unit);
	TrackingMarker* GetTrackingMarker(Unit* unit);
	void RemoveTrackingMarker(TrackingMarker* trackingMarker);

	void AddShootingAndSmokeMarkers(const Shooting& shooting);

	void AddShootingMarker(const Shooting& shooting);
	ShootingCounter* AddShootingMarker(UnitWeapon unitWeapon);
	void RemoveAllShootingMarkers();

	void AddSmokeMarker(const Shooting& shooting);
	SmokeCounter* AddSmokeMarker(UnitWeapon unitWeapon);
	void RemoveAllSmokeMarkers();

	UnitCounter* GetNearestUnitMarker(glm::vec2 position, Player player);
	MovementMarker* GetNearestMovementMarker(glm::vec2 position, Player player);
};



#endif
