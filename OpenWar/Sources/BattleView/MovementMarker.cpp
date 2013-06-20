// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "MovementMarker.h"

#include "BattleModel.h"



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
