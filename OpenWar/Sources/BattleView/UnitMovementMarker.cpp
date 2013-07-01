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
	glm::vec2 finalDestination = _unit->command.GetDestination();

	return _unit->command.path.size() > 1 || glm::length(position - finalDestination) > 8;
}


void UnitMovementMarker::RenderMovementFighters(ColorBillboardRenderer* renderer)
{
	if (!_unit->command.meleeTarget)
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
		if (_unit->command.meleeTarget)
			mode = 2;
		else if (_unit->command.running)
			mode = 1;

		Path(renderer, mode, _unit->command.path);
	}
}


