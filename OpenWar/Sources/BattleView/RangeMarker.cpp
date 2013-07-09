// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "RangeMarker.h"
#include "GradientRenderer.h"


RangeMarker::RangeMarker(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit)
{
}


void RangeMarker::Render(GradientTriangleStripRenderer* renderer)
{
	if (_unit->command.missileTarget != nullptr)
	{
		RenderMissileTarget(renderer, _unit->command.missileTarget->state.center);
	}
	else if (_unit->stats.maximumRange > 0 && _unit->state.unitMode != UnitModeMoving && !_unit->state.IsRouting())
	{
		RenderMissileRange(renderer, _unit->state.center, _unit->state.direction, 20, _unit->stats.maximumRange);
	}
}


void RangeMarker::RenderMissileRange(GradientTriangleStripRenderer* renderer, glm::vec2 position, float direction, float minimumRange, float maximumRange)
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



void RangeMarker::RenderMissileTarget(GradientTriangleStripRenderer* renderer, glm::vec2 target)
{
	glm::vec4 c0 = glm::vec4(255, 64, 64, 0) / 255.0f;
	glm::vec4 c1 = glm::vec4(255, 64, 64, 24) / 255.0f;

	glm::vec2 left = _unit->formation.GetFrontLeft(_unit->state.center);
	glm::vec2 right = left + _unit->formation.towardRight * (float)_unit->formation.numberOfFiles;
	glm::vec2 p;

	const float thickness = 4;
	const float radius_outer = 16;
	const float radius_inner = radius_outer - thickness;
	float radius_left = glm::distance(left, target);
	float radius_right = glm::distance(right, target);

	float angle_left = angle(left - target);
	float angle_right = angle(right - target);
	if (angle_left < angle_right)
		angle_left += 2 * glm::pi<float>();

	glm::vec2 delta = thickness * vector2_from_angle(angle_left + glm::half_pi<float>());
	renderer->AddVertex(GetPosition(left + delta), c0, true);
	renderer->AddVertex(GetPosition(left), c1);

	for (int i = 7; i >= 1; --i)
	{
		float r = i / 8.0f * radius_left;
		if (r > radius_outer)
		{
			p = target + r * vector2_from_angle(angle_left);
			renderer->AddVertex(GetPosition(p + delta), c0);
			renderer->AddVertex(GetPosition(p), c1);
		}
	}

	p = target + radius_outer * vector2_from_angle(angle_left);
	renderer->AddVertex(GetPosition(p + delta), c0);
	renderer->AddVertex(GetPosition(p), c1);

	p = target + radius_inner * vector2_from_angle(angle_left);
	renderer->AddVertex(GetPosition(p + delta), c0);
	renderer->AddVertex(GetPosition(p), c0);

	for (int i = 0; i <= 24; ++i)
	{
		float a = angle_left - i * (angle_left - angle_right) / 24;
		renderer->AddVertex(GetPosition(target + radius_outer * vector2_from_angle(a)), c1, i == 0);
		renderer->AddVertex(GetPosition(target + radius_inner * vector2_from_angle(a)), c0);
	}

	delta = thickness * vector2_from_angle(angle_right - glm::half_pi<float>());
	p = target + radius_inner * vector2_from_angle(angle_right);
	renderer->AddVertex(GetPosition(p + delta), c0);
	renderer->AddVertex(GetPosition(p + delta), c0);

	p = target + radius_outer * vector2_from_angle(angle_right);
	renderer->AddVertex(GetPosition(p), c1);
	renderer->AddVertex(GetPosition(p + delta), c0);

	for (int i = 1; i <= 7; ++i)
	{
		float r = i / 8.0f * radius_right;
		if (r > radius_outer)
		{
			p = target + r * vector2_from_angle(angle_right);
			renderer->AddVertex(GetPosition(p), c1);
			renderer->AddVertex(GetPosition(p + delta), c0);
		}
	}

	renderer->AddVertex(GetPosition(right), c1);
	renderer->AddVertex(GetPosition(right + delta), c0);
}



glm::vec3 RangeMarker::GetPosition(glm::vec2 p) const
{
	glm::vec3 result = _battleModel->terrainSurface->GetPosition(p, 1);
	if (result.z < 0.5f)
		result.z = 0.5f;
	return result;
}
