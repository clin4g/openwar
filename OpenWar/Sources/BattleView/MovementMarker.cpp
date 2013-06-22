// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "MovementMarker.h"
#include "BattleModel.h"
#include "ColorBillboardRenderer.h"



MovementMarker::MovementMarker(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit)
{
}


MovementMarker::~MovementMarker()
{
}


bool MovementMarker::Animate(float seconds)
{
	if (_battleModel->GetUnit(_unit->unitId) == 0 || _unit->state.IsRouting())
		return false;

	glm::vec2 position = _unit->state.center;
	glm::vec2 finalDestination = _unit->movement.GetFinalDestination();

	return _unit->movement.path.size() > 1 || glm::length(position - finalDestination) > 8;
}


void MovementMarker::RenderMovementFighters(ColorBillboardRenderer* renderer)
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
