// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEMODEL_H
#define BATTLEMODEL_H

#include "SmoothTerrainModel.h"
#include "sprite.h"
#include "vertexbuffer.h"
#include "SimulationState.h"


class BattleModel;


class CasualtyMarker
{
public:
	struct Casualty
	{
		glm::vec3 position;
		Player player;
		UnitPlatform platform;
		float time;
		int seed;

		Casualty(glm::vec3 position_, Player player_, UnitPlatform platform_) :
		position(position_), player(player_), platform(platform_), time(0), seed(rand() & 0x7fff) { }
	};
	std::vector<Casualty> casualties;

public:
	CasualtyMarker();
	~CasualtyMarker();

	void AddCasualty(glm::vec3 position, Player player, UnitPlatform platform);
	bool Animate(float seconds);
};


class MovementMarker
{
public:
	BattleModel* _battleModel;
	Unit* _unit;

public:
	MovementMarker(BattleModel* battleModel, Unit* unit);
	~MovementMarker();

	bool Animate(float seconds);
};


class RangeMarker
{
public:
	BattleModel* _battleModel;
	Unit* _unit;

public:
	RangeMarker(BattleModel* battleModel, Unit* unit);
	~RangeMarker();

	bool Animate(float seconds);
};


class ShootingMarker
{
public:
	struct Projectile
	{
		glm::vec3 position1;
		glm::vec3 position2;
		float time;
		float duration;

		Projectile() :
		position1(),
		position2(),
		time(0),
		duration(0) { }
	};

	UnitWeapon _unitWeapon;
	std::vector<Projectile> _projectiles;
	int _soundCookie;
	bool _impacted;

public:
	ShootingMarker(UnitWeapon unitWeapon);
	~ShootingMarker();

	bool Animate(float seconds);
	void AddProjectile(glm::vec3 position1, glm::vec3 position2, float delay, float duration);
};


class SmokeMarker
{
public:
	struct Particle
	{
		glm::vec3 position;
		glm::vec3 velocity;
		float time;

		Particle() :
		position(),
		velocity(),
		time(0) { }
	};

	UnitWeapon _unitWeapon;
	std::vector<Particle> particles;
	int _soundCookie;
	bool _impacted;

public:
	SmokeMarker(UnitWeapon unitWeapon);
	~SmokeMarker();

	bool Animate(float seconds);
	void AddParticle(glm::vec3 position1, glm::vec3 position2, float delay);
};


class TrackingMarker
{
public:
	Unit* _unit;

	Unit* _destinationUnit;
	glm::vec2 _destination;
	bool _hasDestination;

	Unit* _orientationUnit;
	glm::vec2 _orientation;
	bool _hasOrientation;

	std::vector<glm::vec2> _path;
	bool _running;

public:
	TrackingMarker(Unit* unit);
	~TrackingMarker();

	glm::vec2* GetDestination() { return _hasDestination ? &_destination : 0; }
	void SetDestination(glm::vec2* value)
	{
		if (value != 0) _destination = *value;
		_hasDestination = value != 0;
	}

	glm::vec2* GetDestinationX()
	{
		if (_destinationUnit) return &_destinationUnit->state.center;
		else if (_hasDestination) return &_destination;
		else return 0;
	}

	glm::vec2* GetOrientation() { return _hasOrientation ? &_orientation : 0; }
	void SetOrientation(glm::vec2* value)
	{
		if (value != 0) _orientation = *value;
		_hasOrientation = value != 0;
	}

	glm::vec2* GetOrientationX()
	{
		if (_orientationUnit) return &_orientationUnit->state.center;
		else if (_hasOrientation) return &_orientation;
		else return 0;
	}
};


class UnitMarker
{
public:
	BattleModel* _battleModel;
	Unit* _unit;
	float _routingTimer;

public:
	UnitMarker(BattleModel* battleModel, Unit* unit);
	~UnitMarker();

	bool Animate(float seconds);
};


class BattleModel
{
public:
	glm::vec2 _mapSize;
	Player _player;
	CasualtyMarker* _casualtyMarker;
	std::vector<UnitMarker*> _unitMarkers;
	std::vector<MovementMarker*> _movementMarkers;
	std::vector<TrackingMarker*> _trackingMarkers;
	std::vector<RangeMarker*> _rangeMarkers;
	std::vector<ShootingMarker*> _shootingMarkers;
	std::vector<SmokeMarker*> _smokeMarkers;

public:
	SimulationState* _simulationState;

	BattleModel(SimulationState* simulationState);
	~BattleModel();

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
	ShootingMarker* AddShootingMarker(UnitWeapon unitWeapon);
	void RemoveAllShootingMarkers();

	void AddSmokeMarker(const Shooting& shooting);
	SmokeMarker* AddSmokeMarker(UnitWeapon unitWeapon);
	void RemoveAllSmokeMarkers();

	UnitMarker* GetNearestUnitMarker(glm::vec2 position, Player player);
	MovementMarker* GetNearestMovementMarker(glm::vec2 position, Player player);
};



#endif
