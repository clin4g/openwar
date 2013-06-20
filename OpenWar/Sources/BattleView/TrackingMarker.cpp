// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TrackingMarker.h"


TrackingMarker::TrackingMarker(Unit* unit) :
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
