// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TrackingMarker.h"
#import "ColorBillboardRenderer.h"


TrackingMarker::TrackingMarker(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit),
_destinationUnit(0),
_destination(_unit->state.center),
_hasDestination(false),
_orientationUnit(0),
_orientation(),
_hasOrientation(false),
_running(false)
{

}


TrackingMarker::~TrackingMarker()
{
}



static glm::vec2 DestinationXXX(TrackingMarker* marker)
{
	return marker->_destinationUnit ? marker->_destinationUnit->state.center
			: marker->_path.size() != 0 ? *(marker->_path.end() - 1)
					: marker->_hasDestination ? marker->_destination
							: marker->_unit->state.center;
}



void TrackingMarker::RenderTrackingFighters(ColorBillboardRenderer* renderer)
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
