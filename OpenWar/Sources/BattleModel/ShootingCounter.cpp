// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ShootingCounter.h"
#include "SoundPlayer.h"



ShootingCounter::ShootingCounter(UnitWeapon unitWeapon) :
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


ShootingCounter::~ShootingCounter()
{
}


void ShootingCounter::AddProjectile(glm::vec3 position1, glm::vec3 position2, float delay, float duration)
{
	Projectile projectile;
	projectile.position1 = position1;
	projectile.position2 = position2;
	projectile.time = -delay;
	projectile.duration = duration;

	_projectiles.push_back(projectile);
}


bool ShootingCounter::Animate(float seconds)
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
