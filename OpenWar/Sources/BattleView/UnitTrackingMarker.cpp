// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitTrackingMarker.h"
#include "ColorBillboardRenderer.h"
#include "TextureBillboardRenderer.h"
#include "GradientRenderer.h"
#include "TextureRenderer.h"
#include "BattleView.h"
#include "PathRenderer.h"


UnitTrackingMarker::UnitTrackingMarker(BattleModel* battleModel, Unit* unit) : UnitMarker(battleModel, unit),
_meleeTarget(0),
_destination(_unit->state.center),
_hasDestination(false),
_missileTarget(0),
_orientation(),
_hasOrientation(false),
_renderOrientation(false),
_running(false)
{

}


UnitTrackingMarker::~UnitTrackingMarker()
{
}


float UnitTrackingMarker::GetFacing() const
{
	glm::vec2 orientation = _missileTarget ? _missileTarget->state.center : _orientation;
	return angle(orientation - DestinationXXX());
}


void UnitTrackingMarker::RenderTrackingFighters(ColorBillboardRenderer* renderer)
{
	if (!_meleeTarget && !_missileTarget)
	{
		bool isBlue = _unit->player == _battleModel->bluePlayer;
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 16) / 255.0f : glm::vec4(255, 0, 0, 16) / 255.0f;

		glm::vec2 destination = DestinationXXX();
		//glm::vec2 orientation = _missileTarget ? _missileTarget->state.center : _orientation;

		Formation formation = _unit->formation;
		formation.SetDirection(GetFacing());

		glm::vec2 frontLeft = formation.GetFrontLeft(destination);

		for (Fighter* fighter = _unit->fighters, * end = fighter + _unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 offsetRight = formation.towardRight * (float)Unit::GetFighterFile(fighter);
			glm::vec2 offsetBack = formation.towardBack * (float)Unit::GetFighterRank(fighter);

			renderer->AddBillboard(_battleModel->terrainSurface->GetPosition(frontLeft + offsetRight + offsetBack, 0.5), color, 3.0);
		}
	}
}



void UnitTrackingMarker::RenderTrackingMarker(TextureBillboardRenderer* renderer)
{
	if (_meleeTarget == nullptr)
	{
		glm::vec2 destination = DestinationXXX();
		glm::vec3 position = _battleModel->terrainSurface->GetPosition(destination, 0);
		glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
		glm::vec2 texcoord = texsize * glm::vec2(_unit->player != _battleModel->bluePlayer ? 4 : 3, 0);

		renderer->AddBillboard(position, 32, affine2(texcoord, texcoord + texsize));
	}
}


void UnitTrackingMarker::AppendFacingMarker(TextureTriangleRenderer* renderer, BattleView* battleView)
{
	if (_path.empty())
		return;

	float facing = GetFacing();

	bounds2f b = battleView->GetUnitFacingMarkerBounds(_path.back(), facing);
	glm::vec2 p = b.center();
	float size = b.height();
	float direction = facing - battleView->GetCameraFacing();
	if (battleView->GetFlip())
		direction += glm::pi<float>();

	glm::vec2 d1 = size * vector2_from_angle(direction - glm::half_pi<float>() / 2.0f);
	glm::vec2 d2 = glm::vec2(d1.y, -d1.x);
	glm::vec2 d3 = glm::vec2(d2.y, -d2.x);
	glm::vec2 d4 = glm::vec2(d3.y, -d3.x);

	float txs = 0.0625f;
	float tx1 = 1 * txs;
	float tx2 = tx1 + txs;

	float ty1 = _unit->player == battleView->GetBattleModel()->bluePlayer ? 0.0f : 0.5f;
	float ty2 = _unit->player == battleView->GetBattleModel()->bluePlayer ? 0.5f : 1.0f;

	renderer->AddVertex(glm::vec3(p + d1, 0), glm::vec2(tx1, ty1));
	renderer->AddVertex(glm::vec3(p + d2, 0), glm::vec2(tx1, ty2));
	renderer->AddVertex(glm::vec3(p + d3, 0), glm::vec2(tx2, ty2));

	renderer->AddVertex(glm::vec3(p + d3, 0), glm::vec2(tx2, ty2));
	renderer->AddVertex(glm::vec3(p + d4, 0), glm::vec2(tx2, ty1));
	renderer->AddVertex(glm::vec3(p + d1, 0), glm::vec2(tx1, ty1));
}


void UnitTrackingMarker::RenderTrackingShadow(TextureBillboardRenderer* renderer)
{
	glm::vec2 destination = DestinationXXX();
	glm::vec3 position = _battleModel->terrainSurface->GetPosition(destination, 0);

	renderer->AddBillboard(position, 32, affine2(glm::vec2(0, 0), glm::vec2(1, 1)));
}



void UnitTrackingMarker::RenderTrackingPath(GradientTriangleRenderer* renderer)
{
	if (!_path.empty())
	{
		int mode = 0;
		if (_meleeTarget)
			mode = 2;
		else if (_running)
			mode = 1;

		TerrainSurface* terrainSurface = _battleModel->terrainSurface;
		std::function<glm::vec3(glm::vec2)> getPosition = [terrainSurface](glm::vec2 p) { return terrainSurface->GetPosition(p, 1); };
		PathRenderer::Path(renderer, _path, getPosition, mode);
	}
}


void UnitTrackingMarker::RenderOrientation(GradientTriangleRenderer* renderer)
{
	if (_renderOrientation && _hasOrientation && !_path.empty())
	{
		glm::vec2 center = _path.back();
		glm::vec2 diff = _orientation - center;
		if (glm::length(diff) < 0.1)
			return;

		glm::vec2 dir = glm::normalize(diff);
		glm::vec2 left = glm::vec2(dir.y, -dir.x);

		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(center + 10.0f * left, 0), glm::vec4(0, 0, 0, 0));
		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(_orientation + 20.0f * dir, 0), glm::vec4(0, 0, 0, 0.1f));
		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(center - 10.0f * left, 0), glm::vec4(0, 0, 0, 0));
	}
}
