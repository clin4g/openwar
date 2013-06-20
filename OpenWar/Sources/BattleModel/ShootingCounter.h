// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ShootingMarker_H
#define ShootingMarker_H

#include "SimulationState.h"


class ShootingCounter
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
	ShootingCounter(UnitWeapon unitWeapon);
	~ShootingCounter();

	bool Animate(float seconds);
	void AddProjectile(glm::vec3 position1, glm::vec3 position2, float delay, float duration);
};


#endif
