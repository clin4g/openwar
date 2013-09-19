// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/constants.hpp>

#include "UnitCounter.h"
#include "BattleModel.h"
#include "PlainRenderer.h"
#include "TextureBillboardRenderer.h"
#include "TextureRenderer.h"
#include "BattleView.h"


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


void UnitCounter::AppendUnitMarker(TextureBillboardRenderer* renderer1, TextureBillboardRenderer* renderer2, bool flip)
{
	bool routingIndicator = false;
	float routingBlinkTime = _unit->state.GetRoutingBlinkTime();

	if (_unit->state.IsRouting())
	{
		routingIndicator = true;
	}
	else if (routingBlinkTime != 0 && 0 <= _routingTimer && _routingTimer < 0.2f)
	{
		routingIndicator = true;
	}

	int state = 0;
	if (routingIndicator)
		state = 2;
	else if (_unit->player != _battleModel->bluePlayer)
		state = 1;

	glm::vec3 position = _battleModel->terrainSurface->GetPosition(_unit->state.center, 0);
	glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
	glm::vec2 texcoord1 = texsize * glm::vec2(state, 0);
	glm::vec2 texcoord2 = texsize * glm::vec2((int)_unit->stats.unitWeapon, 1 + (int)_unit->stats.unitPlatform);

	if (flip)
	{
		texcoord1 += texsize;
		texcoord2 += texsize;
		texsize = -texsize;
	}

	renderer1->AddBillboard(position, 32, affine2(texcoord1, texcoord1 + texsize));
	renderer2->AddBillboard(position, 32, affine2(texcoord2, texcoord2 + texsize));
}


void UnitCounter::AppendFacingMarker(TextureTriangleRenderer* renderer, BattleView* battleView)
{
	if (_unit->state.unitMode != UnitModeStanding
		|| _unit->command.meleeTarget != nullptr
		|| _unit->state.IsRouting())
	{
		return;
	}

	int index = 0;
	if (_unit->command.holdFire)
	{
		index = 11;
	}
	else if (_unit->state.loadingDuration != 0)
	{
		index = 2 + (int)glm::round(9 * _unit->state.loadingTimer / _unit->state.loadingDuration);
		index = glm::min(10, index);
	}

	float txs = 0.0625f;
	float tx1 = index * txs;
	float tx2 = tx1 + txs;

	float ty1 = _unit->player == battleView->GetBattleModel()->bluePlayer ? 0.0f : 0.5f;
	float ty2 = _unit->player == battleView->GetBattleModel()->bluePlayer ? 0.5f : 1.0f;

	bounds2f bounds = battleView->GetUnitCurrentFacingMarkerBounds(_unit);
	glm::vec2 p = bounds.center();
	float size = bounds.height();
	float direction = index != 0 ? -glm::half_pi<float>() : (_unit->state.direction - battleView->GetCameraFacing());
	if (battleView->GetFlip())
		direction += glm::pi<float>();

	glm::vec2 d1 = size * vector2_from_angle(direction - glm::half_pi<float>() / 2.0f);
	glm::vec2 d2 = glm::vec2(d1.y, -d1.x);
	glm::vec2 d3 = glm::vec2(d2.y, -d2.x);
	glm::vec2 d4 = glm::vec2(d3.y, -d3.x);

	renderer->AddVertex(glm::vec3(p + d1, 0), glm::vec2(tx1, ty2));
	renderer->AddVertex(glm::vec3(p + d2, 0), glm::vec2(tx1, ty1));
	renderer->AddVertex(glm::vec3(p + d3, 0), glm::vec2(tx2, ty1));

	renderer->AddVertex(glm::vec3(p + d3, 0), glm::vec2(tx2, ty1));
	renderer->AddVertex(glm::vec3(p + d4, 0), glm::vec2(tx2, ty2));
	renderer->AddVertex(glm::vec3(p + d1, 0), glm::vec2(tx1, ty2));
}


void UnitCounter::AppendFighterWeapons(PlainLineRenderer* renderer)
{
	if (_unit->stats.weaponReach > 0)
	{
		for (Fighter* fighter = _unit->fighters, * end = fighter + _unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 p1 = fighter->state.position;
			glm::vec2 p2 = p1 + _unit->stats.weaponReach * vector2_from_angle(fighter->state.direction);

			renderer->AddLine(
					_battleModel->terrainSurface->GetPosition(p1, 1),
					_battleModel->terrainSurface->GetPosition(p2, 1));
		}
	}
}


void UnitCounter::AppendFighterBillboards(BillboardModel* billboardModel)
{
	for (Fighter* fighter = _unit->fighters, * end = fighter + _unit->fightersCount; fighter != end; ++fighter)
	{
		float size = 2.0;
		int shape = 0;
		switch (_unit->stats.unitPlatform)
		{
			case UnitPlatformCav:
			case UnitPlatformGen:
				shape = _unit->player == _battleModel->bluePlayer ? billboardModel->_billboardShapeFighterCavBlue : billboardModel->_billboardShapeFighterCavRed;
				size = 3.0;
				break;

			case UnitPlatformSam:
				shape = _unit->player == _battleModel->bluePlayer ? billboardModel->_billboardShapeFighterSamBlue : billboardModel->_billboardShapeFighterSamRed;
				size = 2.0;
				break;

			case UnitPlatformAsh:
				shape = _unit->player == _battleModel->bluePlayer ? billboardModel->_billboardShapeFighterAshBlue : billboardModel->_billboardShapeFighterAshRed;
				size = 2.0;
				break;
		}


		const float adjust = 0.5 - 2.0 / 64.0; // place texture 2 texels below ground
		glm::vec3 p = _battleModel->terrainSurface->GetPosition(fighter->state.position, adjust * size);
		float facing = glm::degrees(fighter->state.direction);
		billboardModel->dynamicBillboards.push_back(Billboard(p, facing, size, shape));
	}
}
