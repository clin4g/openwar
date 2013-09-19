// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/constants.hpp>

#include "UnitMovementMarker.h"
#include "BattleModel.h"
#include "ColorBillboardRenderer.h"
#include "TextureBillboardRenderer.h"
#include "TextureRenderer.h"
#include "BattleView.h"
#include "PathRenderer.h"



UnitMovementMarker::UnitMovementMarker(BattleModel* battleModel, Unit* unit) : UnitMarker(battleModel, unit)
{
}


UnitMovementMarker::~UnitMovementMarker()
{
}


bool UnitMovementMarker::Animate(float seconds)
{
	return _battleModel->GetUnit(_unit->unitId) != nullptr
		&& !_unit->state.IsRouting()
		&& MovementRules::Length(_unit->command.path) > 8;
}


void UnitMovementMarker::RenderMovementMarker(TextureBillboardRenderer* renderer)
{
	glm::vec2 finalDestination = _unit->command.GetDestination();
	if (_unit->command.path.size() > 1 || glm::length(_unit->state.center - finalDestination) > 25)
	{
		if (_unit->command.meleeTarget == nullptr)
		{
			glm::vec3 position = _battleModel->terrainSurface->GetPosition(finalDestination, 0.5);
			glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
			glm::vec2 texcoord = texsize * glm::vec2(_unit->player != _battleModel->bluePlayer ? 4 : 3, 0);

			renderer->AddBillboard(position, 32, affine2(texcoord, texcoord + texsize));
		}
	}
}


void UnitMovementMarker::AppendFacingMarker(TextureTriangleRenderer* renderer, BattleView* battleView)
{
	if (_unit->state.unitMode != UnitModeMoving)
		return;

	bounds2f b = battleView->GetUnitFutureFacingMarkerBounds(_unit);
	glm::vec2 p = b.center();
	float size = b.height();
	float direction = _unit->command.facing - battleView->GetCameraFacing();
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


void UnitMovementMarker::RenderMovementFighters(ColorBillboardRenderer* renderer)
{
	if (_unit->command.meleeTarget == nullptr)
	{
		bool isBlue = _unit->player == _battleModel->bluePlayer;
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 32) / 255.0f : glm::vec4(255, 0, 0, 32) / 255.0f;

		glm::vec2 finalDestination = _unit->command.GetDestination();

		Formation formation = _unit->formation;
		formation.SetDirection(_unit->command.facing);

		glm::vec2 frontLeft = formation.GetFrontLeft(finalDestination);

		for (Fighter* fighter = _unit->fighters, * end = fighter + _unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 offsetRight = formation.towardRight * (float)Unit::GetFighterFile(fighter);
			glm::vec2 offsetBack = formation.towardBack * (float)Unit::GetFighterRank(fighter);

			renderer->AddBillboard(_battleModel->terrainSurface->GetPosition(frontLeft + offsetRight + offsetBack, 0.5), color, 3.0);
		}
	}
}


void UnitMovementMarker::RenderMovementPath(GradientTriangleRenderer* renderer)
{
	if (!_unit->command.path.empty())
	{
		int mode = 0;
		if (_unit->command.meleeTarget != nullptr)
			mode = 2;
		else if (_unit->command.running)
			mode = 1;

		TerrainSurface* terrainSurface = _battleModel->terrainSurface;
		PathRenderer pathRenderer([terrainSurface](glm::vec2 p) { return terrainSurface->GetPosition(p, 1); });
		pathRenderer.Path(renderer, _unit->command.path, mode);
	}
}
