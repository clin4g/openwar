// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMarker_H
#define UnitMarker_H

#include "BattleModel.h"


class UnitMarker
{
public:
	BattleModel* _battleModel;
	Unit* _unit;
	float _routingTimer;

public:
	UnitMarker(BattleModel* battleModel, Unit* unit);
	~UnitMarker();

	bool Animate(float seconds);
};


#endif
