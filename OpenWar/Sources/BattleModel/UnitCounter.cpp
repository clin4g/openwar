// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitCounter.h"
#include "BattleRendering.h"
#include "BattleModel.h"
#include "BillboardRenderer.h"


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


void UnitCounter::AppendFighterBillboards(BillboardModel* billboardModel)
{
	for (Fighter* fighter = _unit->fighters, * end = fighter + _unit->fightersCount; fighter != end; ++fighter)
	{
		float size = 2.0;
		//float diff = angle_difference(GetCameraFacing(), fighter->state.direction);
		//float absdiff = fabsf(diff);

		int shape = 0;
		//int i = unit->player == Player2 ? 2 : 1;
		//int j = 0;
		switch (_unit->stats.unitPlatform)
		{
			case UnitPlatformCav:
			case UnitPlatformGen:
				shape = _unit->player == Player2 ? billboardModel->_billboardShapeFighterCavRed : billboardModel->_billboardShapeFighterCavBlue;
				size = 3.0;
				/*if (absdiff < 0.33 * M_PI)
					j = 5;
				else if (absdiff < 0.66 * M_PI)
					j = 6;
				else
					j = 7;*/
				break;

			case UnitPlatformSam:
				shape = _unit->player == Player2 ? billboardModel->_billboardShapeFighterSamRed : billboardModel->_billboardShapeFighterSamBlue;
				size = 2.0;
				//j = absdiff < M_PI_2 ? 2 : 1;
				break;

			case UnitPlatformAsh:
				shape = _unit->player == Player2 ? billboardModel->_billboardShapeFighterAshRed : billboardModel->_billboardShapeFighterAshBlue;
				size = 2.0;
				//j = absdiff < M_PI_2 ? 4 : 3;
				break;
		}


		const float adjust = 0.5 - 2.0 / 64.0; // place texture 2 texels below ground
		glm::vec3 p = _battleModel->terrainSurface->GetPosition(fighter->state.position, adjust * size);
		float facing = glm::degrees(fighter->state.direction);
		billboardModel->dynamicBillboards.push_back(Billboard(p, facing, size, shape));
	}
}
