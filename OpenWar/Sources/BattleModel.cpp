/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "BattleModel.h"
#include "BattleView.h"

#include "SoundPlayer.h"



CasualtyMarker::CasualtyMarker()
{
}


CasualtyMarker::~CasualtyMarker()
{
}


void CasualtyMarker::AddCasualty(glm::vec3 position, Player player, UnitPlatform platform)
{
	casualties.push_back(Casualty(position, player, platform));
}


bool CasualtyMarker::Animate(float seconds)
{
	for (Casualty& casualty : casualties)
		casualty.time += seconds;

	return true;
}


/***/







MovementMarker::MovementMarker(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit)
{
}


MovementMarker::~MovementMarker()
{
}


bool MovementMarker::Animate(float seconds)
{
	if (_battleModel->_simulationState->GetUnit(_unit->unitId) == 0 || _unit->state.IsRouting())
		return false;

	glm::vec2 position = _unit->state.center;
	glm::vec2 finalDestination = _unit->movement.GetFinalDestination();

	return _unit->movement.path.size() > 1 || glm::length(position - finalDestination) > 8;
}


/***/



RangeMarker::RangeMarker(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit)
{
	Animate(0);
}


RangeMarker::~RangeMarker()
{
}


bool RangeMarker::Animate(float seconds)
{
	if (_battleModel->_simulationState->GetUnit(_unit->unitId) == 0)
		return false;

	return true;
}


/***/



ShootingMarker::ShootingMarker(UnitWeapon unitWeapon) :
_unitWeapon(unitWeapon),
_projectiles(),
_soundCookie(0),
_impacted(false)
{
	if (unitWeapon == UnitWeaponArq)
		SoundPlayer::singleton->PlayMatchlock();
	else
		SoundPlayer::singleton->PlayArrows();
}


ShootingMarker::~ShootingMarker()
{
}


void ShootingMarker::AddProjectile(glm::vec3 position1, glm::vec3 position2, float delay, float duration)
{
	Projectile projectile;
	projectile.position1 = position1;
	projectile.position2 = position2;
	projectile.time = -delay;
	projectile.duration = duration;

	_projectiles.push_back(projectile);
}


bool ShootingMarker::Animate(float seconds)
{
	bool alive = false;
	bool impact = false;

	for (Projectile& projectile : _projectiles)
	{
		if (projectile.time < 0)
		{
			projectile.time += seconds;
			alive = true;
		}
		else if (projectile.time < projectile.duration)
		{
			projectile.time += seconds;
			if (projectile.time > projectile.duration)
				projectile.time = projectile.duration;
			alive = true;
		}
		else if (projectile.time == projectile.duration)
		{
			projectile.time += 1;
			alive = true;
			impact = true;
		}
		else
		{
		}
	}

	if (impact && !_impacted)
	{
		SoundPlayer::singleton->PlayGrunts();
		_impacted = true;
	}

	if (!alive && _soundCookie != 0)
	{
		SoundPlayer::singleton->Stop(_soundCookie);
	}

	return alive;
}


/***/



SmokeMarker::SmokeMarker(UnitWeapon unitWeapon) :
_unitWeapon(unitWeapon),
particles(),
_soundCookie(0),
_impacted(false)
{
	if (unitWeapon == UnitWeaponArq)
		SoundPlayer::singleton->PlayMatchlock();
	else
		SoundPlayer::singleton->PlayArrows();
}


SmokeMarker::~SmokeMarker()
{
}


void SmokeMarker::AddParticle(glm::vec3 position1, glm::vec3 position2, float delay)
{
	glm::vec3 dir = glm::normalize(position2 - position1);

	Particle particle;
	particle.position = position1 + glm::vec3(0, 0, 1.5) + 2.0f * dir;
	particle.velocity = 4.0f * dir;
	particle.time = -delay;

	particles.push_back(particle);
}


bool SmokeMarker::Animate(float seconds)
{
	float duration = 3;
	bool alive = false;

	for (Particle& particle : particles)
	{
		if (particle.time < 0)
		{
			particle.time += seconds;
		}
		else
		{
			particle.time += seconds / duration;
			particle.position += seconds * particle.velocity;
			particle.velocity *= exp2f(-4 * seconds);
		}

		if (particle.time < 1)
			alive = true;
	}

	return alive;
}


/***/



TrackingMarker::TrackingMarker(Unit* unit) :
_unit(unit),
_destinationUnit(0),
_destination(_unit->state.center),
_hasDestination(false),
_orientationUnit(0),
_orientation(),
_hasOrientation(false),
_running(false)
{

}


TrackingMarker::~TrackingMarker()
{
}


/***/






UnitMarker::UnitMarker(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit),
_routingTimer(0)
{
}


UnitMarker::~UnitMarker()
{
}


bool UnitMarker::Animate(float seconds)
{
	if (_battleModel->_simulationState->GetUnit(_unit->unitId) == 0)
		return false;

	float routingBlinkTime = _unit->state.GetRoutingBlinkTime();

	if (!_unit->state.IsRouting() && routingBlinkTime != 0)
	{
		_routingTimer -= seconds;
		if (_routingTimer < 0)
			_routingTimer = routingBlinkTime;
	}

	return true;
}


/***/


BattleModel::BattleModel(SimulationState* simulationState) :
_simulationState(simulationState),
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
	glm::vec3 position = glm::vec3(casualty.position, _simulationState->terrainModel->GetHeight(casualty.position));
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
		glm::vec3 p1 = glm::vec3(projectile.position1, _simulationState->terrainModel->GetHeight(projectile.position1));
		glm::vec3 p2 = glm::vec3(projectile.position2, _simulationState->terrainModel->GetHeight(projectile.position2));
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
		glm::vec3 p1 = glm::vec3(projectile.position1, _simulationState->terrainModel->GetHeight(projectile.position1));
		glm::vec3 p2 = glm::vec3(projectile.position2, _simulationState->terrainModel->GetHeight(projectile.position2));
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
