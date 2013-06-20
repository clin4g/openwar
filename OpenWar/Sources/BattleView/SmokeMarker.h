// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmokeMarker_H
#define SmokeMarker_H

#include "SimulationState.h"


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


#endif
