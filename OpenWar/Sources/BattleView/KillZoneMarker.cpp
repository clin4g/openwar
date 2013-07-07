// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "KillZoneMarker.h"
#include "GradientRenderer.h"


KillZoneMarker::KillZoneMarker(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit)
{
}


void KillZoneMarker::Render(GradientTriangleStripRenderer* renderer)
{
	if (_unit->stats.maximumRange > 0 && _unit->state.unitMode != UnitModeMoving && !_unit->state.IsRouting())
	{
		RenderMissileRange(renderer, _unit->state.center, _unit->state.direction, 20, _unit->stats.maximumRange);
	}

	RenderMeleeReach(renderer);
}


void KillZoneMarker::RenderMeleeReach(GradientTriangleStripRenderer* renderer)
{
	glm::vec4 c1 = glm::vec4(0.9f, 0, 0, 0.15f);
	glm::vec4 c2 = glm::vec4(1, 0.25f, 0.25f, 0);

	float kfront = glm::max(2.4f, _unit->stats.weaponReach);
	float kbase = -0.6f;
	float kleft = 1.5f;

	for (int i = 0; i < _unit->fightersCount; i += _unit->formation.numberOfRanks)
	{
		glm::vec2 p = _unit->fighters[i].state.position;
		glm::vec2 dir = vector2_from_angle(_unit->fighters[i].state.direction);
		glm::vec2 left = kleft * glm::vec2(-dir.y, dir.x);
		glm::vec2 front = kfront * dir;
		glm::vec2 base = kbase * dir;

		glm::vec3 p1 = GetPosition(p + base + left);
		glm::vec3 p2 = GetPosition(p + front);
		glm::vec3 p3 = GetPosition(p + base - left);

		renderer->AddVertex(p1, c2, true);
		renderer->AddVertex(p2, c1);
		renderer->AddVertex(p3, c2);
	}
}


void KillZoneMarker::RenderMissileRange(GradientTriangleStripRenderer* renderer, glm::vec2 position, float direction, float minimumRange, float maximumRange)
{
	const float thickness = 8;
	const float two_pi = 2 * (float)M_PI;
	glm::vec4 c0 = glm::vec4(255, 64, 64, 0) / 255.0f;
	glm::vec4 c1 = glm::vec4(255, 64, 64, 24) / 255.0f;

	float d = direction - two_pi / 8;
	glm::vec2 p2 = maximumRange * vector2_from_angle(d - 0.03f);
	glm::vec2 p3 = minimumRange * vector2_from_angle(d);
	glm::vec2 p4 = maximumRange * vector2_from_angle(d);
	glm::vec2 p5 = (maximumRange - thickness) * vector2_from_angle(d);
	glm::vec2 p1 = p3 + (p2 - p4);

	for (int i = 0; i <= 8; ++i)
	{
		float t = i / 8.0f;
		renderer->AddVertex(GetPosition(position + glm::mix(p3, p5, t)), c0);
		renderer->AddVertex(GetPosition(position + glm::mix(p1, p2, t)), c1);
	}

	renderer->AddVertex(GetPosition(position + p4), c1);
	renderer->AddVertex(GetPosition(position + p4), c1);
	renderer->AddVertex(GetPosition(position + p5), c0);

	int n = 10;
	for (int i = 0; i <= n; ++i)
	{
		float k = (i - (float)n / 2) / n;
		d = direction + k * two_pi / 4;
		renderer->AddVertex(GetPosition(position + (maximumRange - thickness) * vector2_from_angle(d)), c0);
		renderer->AddVertex(GetPosition(position + maximumRange * vector2_from_angle(d)), c1);
	}

	d = direction + two_pi / 8;
	p2 = maximumRange * vector2_from_angle(d + 0.03f);
	p3 = minimumRange * vector2_from_angle(d);
	p4 = maximumRange * vector2_from_angle(d);
	p5 = (maximumRange - thickness) * vector2_from_angle(d);
	p1 = p3 + (p2 - p4);

	renderer->AddVertex(GetPosition(position + p4), c1);
	for (int i = 0; i <= 8; ++i)
	{
		float t = i / 8.0f;
		renderer->AddVertex(GetPosition(position + glm::mix(p2, p1, t)), c1);
		renderer->AddVertex(GetPosition(position + glm::mix(p5, p3, t)), c0);
	}
}



glm::vec3 KillZoneMarker::GetPosition(glm::vec2 p) const
{
	glm::vec3 result = _battleModel->terrainSurface->GetPosition(p, 1);
	if (result.z < 0.5f)
		result.z = 0.5f;
	return result;
}
