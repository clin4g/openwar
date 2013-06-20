// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmokeCounter.h"
#include "SoundPlayer.h"



SmokeCounter::SmokeCounter(UnitWeapon unitWeapon) :
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


SmokeCounter::~SmokeCounter()
{
}


void SmokeCounter::AddParticle(glm::vec3 position1, glm::vec3 position2, float delay)
{
	glm::vec3 dir = glm::normalize(position2 - position1);

	Particle particle;
	particle.position = position1 + glm::vec3(0, 0, 1.5) + 2.0f * dir;
	particle.velocity = 4.0f * dir;
	particle.time = -delay;

	particles.push_back(particle);
}


bool SmokeCounter::Animate(float seconds)
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
