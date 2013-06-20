// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TrackingMarker_H
#define TrackingMarker_H

#include "SimulationState.h"


class TrackingMarker
{
public:
	Unit* _unit;

	Unit* _destinationUnit;
	glm::vec2 _destination;
	bool _hasDestination;

	Unit* _orientationUnit;
	glm::vec2 _orientation;
	bool _hasOrientation;

	std::vector<glm::vec2> _path;
	bool _running;

public:
	TrackingMarker(Unit* unit);
	~TrackingMarker();

	glm::vec2* GetDestination() { return _hasDestination ? &_destination : 0; }
	void SetDestination(glm::vec2* value)
	{
		if (value != 0) _destination = *value;
		_hasDestination = value != 0;
	}

	glm::vec2* GetDestinationX()
	{
		if (_destinationUnit) return &_destinationUnit->state.center;
		else if (_hasDestination) return &_destination;
		else return 0;
	}

	glm::vec2* GetOrientation() { return _hasOrientation ? &_orientation : 0; }
	void SetOrientation(glm::vec2* value)
	{
		if (value != 0) _orientation = *value;
		_hasOrientation = value != 0;
	}

	glm::vec2* GetOrientationX()
	{
		if (_orientationUnit) return &_orientationUnit->state.center;
		else if (_hasOrientation) return &_orientation;
		else return 0;
	}
};


#endif
