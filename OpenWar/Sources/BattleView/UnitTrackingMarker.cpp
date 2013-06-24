// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitTrackingMarker.h"
#import "ColorBillboardRenderer.h"
#import "TextureBillboardRenderer.h"


UnitTrackingMarker::UnitTrackingMarker(BattleModel* battleModel, Unit* unit) : UnitMarker(battleModel, unit),
_destinationUnit(0),
_destination(_unit->state.center),
_hasDestination(false),
_orientationUnit(0),
_orientation(),
_hasOrientation(false),
_running(false)
{

}


UnitTrackingMarker::~UnitTrackingMarker()
{
}



static glm::vec2 DestinationXXX(UnitTrackingMarker* marker)
{
	return marker->_destinationUnit ? marker->_destinationUnit->state.center
			: marker->_path.size() != 0 ? *(marker->_path.end() - 1)
					: marker->_hasDestination ? marker->_destination
							: marker->GetUnit()->state.center;
}



void UnitTrackingMarker::RenderTrackingFighters(ColorBillboardRenderer* renderer)
{
	if (!_destinationUnit && !_orientationUnit)
	{
		bool isBlue = _unit->player == _battleModel->bluePlayer;
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 16) / 255.0f : glm::vec4(255, 0, 0, 16) / 255.0f;

		glm::vec2 destination = DestinationXXX(this);
		glm::vec2 orientation = _orientationUnit ? _orientationUnit->state.center : _orientation;

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
	if ((_destinationUnit || _hasDestination) && _destinationUnit == nullptr)
	{
		glm::vec2 destination = DestinationXXX(this);
		glm::vec3 position = _battleModel->terrainSurface->GetPosition(destination, 0);
		glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
		glm::vec2 texcoord = texsize * glm::vec2(_unit->player != _battleModel->bluePlayer ? 4 : 3, 0);

		renderer->AddBillboard(position, 32, affine2(texcoord, texcoord + texsize));
	}
}


void UnitTrackingMarker::RenderTrackingPath(TextureTriangleRenderer* renderer)
{
	if (_path.size() != 0)
	{
		glm::vec2 position = _unit->state.center;

		int mode = 0;
		if (_destinationUnit)
			mode = 2;
		else if (_running)
			mode = 1;

		std::vector<glm::vec2> path(_path);
		if (_destinationUnit != 0)
			path.insert(path.end(), _destinationUnit->state.center);

		Path(renderer, mode, position, path, 0);
	}

}
