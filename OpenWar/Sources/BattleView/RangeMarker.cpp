// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "RangeMarker.h"

#include "BattleModel.h"
#include "BattleContext.h"



RangeMarker::RangeMarker(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit)
{
	Animate(0);
}


RangeMarker::~RangeMarker()
{
}


bool RangeMarker::Animate(float seconds)
{
	if (_battleModel->GetBattleContext()->simulationState->GetUnit(_unit->unitId) == 0)
		return false;

	return true;
}
