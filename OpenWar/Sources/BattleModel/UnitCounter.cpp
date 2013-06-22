// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitCounter.h"
#include "BattleRendering.h"
#include "BattleModel.h"


UnitCounter::UnitCounter(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit),
_routingTimer(0)
{
}


UnitCounter::~UnitCounter()
{
}


bool UnitCounter::Animate(float seconds)
{
	if (_battleModel->GetUnit(_unit->unitId) == 0)
		return false;

	float routingBlinkTime = _unit->state.GetRoutingBlinkTime();

	if (!_unit->state.IsRouting() && routingBlinkTime != 0)
	{
		_routingTimer -= seconds;
		if (_routingTimer < 0)
			_routingTimer = routingBlinkTime;
	}

	return true;
}


void UnitCounter::AppendFighterWeapons(BattleRendering* rendering)
{
	if (_unit->stats.weaponReach > 0)
	{
		for (Fighter* fighter = _unit->fighters, * end = fighter + _unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 p1 = fighter->state.position;
			glm::vec2 p2 = p1 + _unit->stats.weaponReach * vector2_from_angle(fighter->state.direction);

			rendering->_vboFighterWeapons._vertices.push_back(plain_vertex3(_battleModel->terrainSurface->GetPosition(p1, 1)));
			rendering->_vboFighterWeapons._vertices.push_back(plain_vertex3(_battleModel->terrainSurface->GetPosition(p2, 1)));
		}
	}
}
