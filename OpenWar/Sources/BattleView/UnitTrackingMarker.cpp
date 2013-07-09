// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitTrackingMarker.h"
#import "ColorBillboardRenderer.h"
#import "TextureBillboardRenderer.h"
#import "GradientRenderer.h"


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



void UnitTrackingMarker::RenderTrackingFighters(ColorBillboardRenderer* renderer)
{
	if (!_meleeTarget && !_missileTarget)
	{
		bool isBlue = _unit->player == _battleModel->bluePlayer;
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 16) / 255.0f : glm::vec4(255, 0, 0, 16) / 255.0f;

		glm::vec2 destination = DestinationXXX();
		glm::vec2 orientation = _missileTarget ? _missileTarget->state.center : _orientation;

		Formation formation = _unit->formation;
		formation.SetDirection(angle(orientation - destination));

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

		Path(renderer, mode, _path);
	}
}


void UnitTrackingMarker::RenderOrientation(GradientTriangleRenderer* renderer)
{
	if (_renderOrientation && _hasOrientation && !_path.empty())
	{
		glm::vec2 center = _path.back();
		glm::vec2 dir = glm::normalize(_orientation - center);
		glm::vec2 left = glm::vec2(dir.y, -dir.x);

		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(center + 10.0f * left, 0), glm::vec4(0, 0, 0, 0));
		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(_orientation, 0), glm::vec4(0, 0, 0, 0.1f));
		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(center - 10.0f * left, 0), glm::vec4(0, 0, 0, 0));
	}
}
