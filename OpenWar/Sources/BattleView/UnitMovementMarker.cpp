// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitMovementMarker.h"
#include "BattleModel.h"
#include "ColorBillboardRenderer.h"
#include "TextureBillboardRenderer.h"



UnitMovementMarker::UnitMovementMarker(BattleModel* battleModel, Unit* unit) : UnitMarker(battleModel, unit)
{
}


UnitMovementMarker::~UnitMovementMarker()
{
}


bool UnitMovementMarker::Animate(float seconds)
{
	if (_battleModel->GetUnit(_unit->unitId) == 0 || _unit->state.IsRouting())
		return false;

	glm::vec2 position = _unit->state.center;
	glm::vec2 finalDestination = _unit->movement.GetFinalDestination();

	return _unit->movement.path.size() > 1 || glm::length(position - finalDestination) > 8;
}


void UnitMovementMarker::RenderMovementFighters(ColorBillboardRenderer* renderer)
{
	if (!_unit->movement.target)
	{
		bool isBlue = _unit->player == _battleModel->bluePlayer;
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 32) / 255.0f : glm::vec4(255, 0, 0, 32) / 255.0f;

		glm::vec2 finalDestination = _unit->movement.GetFinalDestination();

		Formation formation = _unit->formation;
		formation.SetDirection(_unit->movement.direction);

		glm::vec2 frontLeft = formation.GetFrontLeft(finalDestination);

		for (Fighter* fighter = _unit->fighters, * end = fighter + _unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 offsetRight = formation.towardRight * (float)Unit::GetFighterFile(fighter);
			glm::vec2 offsetBack = formation.towardBack * (float)Unit::GetFighterRank(fighter);

			renderer->AddBillboard(_battleModel->terrainSurface->GetPosition(frontLeft + offsetRight + offsetBack, 0.5), color, 3.0);
		}
	}
}



void UnitMovementMarker::RenderMovementMarker(TextureBillboardRenderer* renderer)
{
	glm::vec2 finalDestination = _unit->movement.GetFinalDestination();
	if (_unit->movement.path.size() > 1 || glm::length(_unit->state.center - finalDestination) > 25)
	{
		if (!_unit->movement.target)
		{
			glm::vec3 position = _battleModel->terrainSurface->GetPosition(finalDestination, 0.5);
			glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
			glm::vec2 texcoord = texsize * glm::vec2(_unit->player != _battleModel->bluePlayer ? 4 : 3, 0);

			renderer->AddBillboard(position, 32, affine2(texcoord, texcoord + texsize));
		}
	}
}



void UnitMovementMarker::RenderMovementPath(TextureTriangleRenderer* renderer)
{
	if (_unit->movement.path.size() != 0)
	{
		glm::vec2 position = _unit->state.center;

		int mode = 0;
		if (_unit->movement.target)
			mode = 2;
		else if (_unit->movement.running)
			mode = 1;

		std::vector<glm::vec2> path(_unit->movement.path);
		if (_unit->movement.target != 0)
			path.insert(path.end(), _unit->movement.target->state.center);

		Path(renderer, mode, position, path, _unit->movement.path_t0);
	}
}


