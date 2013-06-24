// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitTrackingMarker_H
#define UnitTrackingMarker_H

#include "UnitMarker.h"
#include "BattleModel.h"

class ColorBillboardRenderer;
class TextureBillboardRenderer;


class UnitTrackingMarker : public UnitMarker
{
public:
	Unit* _destinationUnit;
	glm::vec2 _destination;
	bool _hasDestination;

	Unit* _orientationUnit;
	glm::vec2 _orientation;
	bool _hasOrientation;

	std::vector<glm::vec2> _path;
	bool _running;

public:
	UnitTrackingMarker(BattleModel* battleModel, Unit* unit);
	~UnitTrackingMarker();

	//glm::vec2* GetDestination() { return _hasDestination ? &_destination : 0; }
	void SetDestination(glm::vec2* value)
	{
		if (value != nullptr) _destination = *value;
		_hasDestination = value !=  nullptr;
	}

	glm::vec2* GetDestinationX()
	{
		if (_destinationUnit) return &_destinationUnit->state.center;
		else if (_hasDestination) return &_destination;
		else return nullptr;
	}

	//glm::vec2* GetOrientation() { return _hasOrientation ? &_orientation : 0; }
	void SetOrientation(glm::vec2* value)
	{
		if (value != nullptr) _orientation = *value;
		_hasOrientation = value != nullptr;
	}

	glm::vec2* GetOrientationX()
	{
		if (_orientationUnit) return &_orientationUnit->state.center;
		else if (_hasOrientation) return &_orientation;
		else return 0;
	}

	void RenderTrackingFighters(ColorBillboardRenderer* renderer);
	void RenderTrackingMarker(TextureBillboardRenderer* renderer);
	void RenderTrackingPath(TextureTriangleRenderer* renderer);
	void RenderTrackingShadow(TextureBillboardRenderer* renderer);
};


#endif
