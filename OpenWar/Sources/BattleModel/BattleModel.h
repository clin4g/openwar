// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEMODEL_H
#define BATTLEMODEL_H

#include "MovementRules.h"
#include "TerrainModel.h"


class BattleModel;
class CasualtyMarker;
class MovementMarker;
class UnitCounter;
class RangeMarker;
class SmokeCounter;
class ShootingCounter;
class TrackingMarker;

struct Fighter;
struct Unit;
struct UnitUpdate;


enum PlayMode
{
	PlayModeNone,
	PlayModeUpperLeft,
	PlayModeUpperRight,
	PlayModeLowerLeft,
	PlayModeLowerRight
};


enum Player
{
	PlayerNone = 0,
	Player1 = 1,
	Player2 = 2
};


inline Player Opponent(Player player)
{
	switch (player)
	{
		case Player1:
			return Player2;
		case Player2:
			return Player1;
		default:
			return PlayerNone;
	}
}


enum UnitPlatform
{
	UnitPlatformCav = 0, // Cavalry
	UnitPlatformGen = 1, // General
	UnitPlatformAsh = 2, // Ashigaru
	UnitPlatformSam = 3  // Samurai
};


enum UnitWeapon
{
	UnitWeaponYari = 0,
	UnitWeaponKata = 1,
	UnitWeaponNagi = 2,
	UnitWeaponBow = 3,
	UnitWeaponArq = 4
};


enum UnitMode
{
	UnitModeInitializing,
	UnitModeStanding,
	UnitModeMoving,
	UnitModeTurning
};


struct Movement
{
	std::vector<glm::vec2> path;
	float path_t0;
	glm::vec2 destination;
	float direction;
	Unit* target;
	bool running;

	Movement();

	glm::vec2 GetCurrentDestination()
	{
		return destination;
	}

	glm::vec2 GetFinalDestination()
	{
		return path.size() != 0 ? *(path.end() - 1) : destination;
	}
};


struct Projectile
{
	glm::vec2 position1;
	glm::vec2 position2;
	float delay;

	Projectile();
};


struct Shooting
{
	UnitWeapon unitWeapon;
	float timeToImpact;
	std::vector<Projectile> projectiles;

	Shooting();
};


struct Casualty
{
	glm::vec2 position;
	Player player;
	UnitPlatform platform;

	Casualty(glm::vec2 position_, Player player_, UnitPlatform platform_) :
	position(position_), player(player_), platform(platform_) { }
};


enum ReadyState
{
	ReadyStateUnready,
	ReadyStateReadying,
	ReadyStatePrepared,
	ReadyStateStriking,
	ReadyStateStunned
};


struct FighterState
{
	// dynamic attributes
	glm::vec2 position;
	ReadyState readyState;
	float readyingTimer;
	float strikingTimer;
	float stunnedTimer;
	Fighter* opponent;

	// intermediate attributes
	glm::vec2 destination;
	glm::vec2 velocity;
	float direction;
	Fighter* meleeTarget;


	FighterState();
};


struct FighterUpdate
{
	unsigned char positionX;
	unsigned char positionY;
};


struct Fighter
{
	// static attributes
	Unit* unit;

	// dynamic attributes
	FighterState state;

	// optimization attributes
	bool terrainForest;
	bool terrainWater;
	glm::vec2 terrainPosition;

	// intermediate attributes
	FighterState nextState;
	bool casualty;


	Fighter();
	FighterUpdate GetFighterUpdate(const UnitUpdate& unitUpdate);
	void SetFighterUpdate(const UnitUpdate& unitUpdate, const FighterUpdate& fighterUpdate);
};


struct UnitStats
{
	UnitPlatform unitPlatform;
	UnitWeapon unitWeapon;
	float weaponReach;
	float trainingLevel;
	float strikingDuration;
	float readyingDuration;
	float fireRate; // shots per minute
	float minimumRange; // minimum fire range in meters
	float maximumRange; // maximum fire range in meters
	float fireAccuracy; // percentage of hit at half range
	float walkingSpeed; // meters per second
	float runningSpeed; // meters per second
	glm::vec2 fighterSize; // x = side-to-side, y = front-to-back
	glm::vec2 spacing;  // x = side-to-side, y = front-to-back

	UnitStats();
};


struct UnitState
{
	// dynamic attributes
	float shootingTimer;
	int shootingCounter;
	float morale;

	// optimization attributes
	UnitMode unitMode;
	glm::vec2 center;
	float direction;
	float influence;

	// intermediate attributes
	int recentCasualties;


	bool IsRouting() const
	{
		float x = morale + influence;
		return x <= 0;
	}

	float GetRoutingBlinkTime() const
	{
		float x = morale + influence;
		return 0 <= x && x < 0.33f ? 0.1f + x * 3 : 0;
	}

	UnitState();
};


struct UnitUpdate
{
	int unitId;
	int fightersCount;
	float morale;

	float minX, maxX, minY, maxY;

	glm::vec2 movementDestination;
	float movementDirection;
	int movementTargetUnitId;
	bool movementRunning;

	UnitUpdate();
};


struct Unit
{
	// static attributes
	int unitId;
	Player player;
	UnitStats stats;
	Fighter* fighters;

	// dynamic attributes
	UnitState state; // updated by AssignNextState()
	int fightersCount; // updated by RemoveCasualties()
	int shootingCounter; // updated by ResolveMissileCombat()
	Formation formation; // updated by UpdateFormation()
	float timeUntilSwapFighters;


	// intermediate attributes
	UnitState nextState; // updated by ComputeNextState()

	// control attributes
	Movement movement; // updated by TouchGesture()
	Unit* missileTarget; // updated by TouchGesture()
	bool missileTargetLocked; // updated by TouchGesture()

	Unit();

	UnitUpdate GetUnitUpdate();
	void SetUnitUpdate(UnitUpdate unitUpdate, BattleModel* battleModel);

	glm::vec2 CalculateUnitCenter();

	float GetSpeed();

	static int GetFighterRank(Fighter* fighter);
	static int GetFighterFile(Fighter* fighter);
	static Fighter* GetFighter(Unit* unit, int rank, int file);
};


class BattleModel : public TerrainModel
{
public:
	int lastUnitId;
	Player winner;
	float time;
	float timeStep;

	std::map<int, Unit*> units;
	std::vector<Shooting> shootings;

	glm::vec2 _mapSize;

	std::vector<UnitCounter*> _unitMarkers;
	std::vector<ShootingCounter*> _shootingMarkers;
	std::vector<SmokeCounter*> _smokeMarkers;

public:
	BattleModel();
	virtual ~BattleModel();

	Unit* GetUnit(int unitId) const
	{
		std::map<int, Unit*>::const_iterator i = units.find(unitId);
		return i != units.end() ? (*i).second : 0;
	}

	bool IsMelee() const;

	Unit* AddUnit(Player player, int numberOfFighters, UnitStats stats, glm::vec2 position);

	static UnitStats GetDefaultUnitStats(UnitPlatform unitPlatform, UnitWeapon unitWeapon);

	void AnimateMarkers(float seconds);

	void InitializeUnitMarkers();

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
