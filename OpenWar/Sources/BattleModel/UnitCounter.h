// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMarker_H
#define UnitMarker_H

#include "BattleModel.h"


class UnitCounter
{
public:
	BattleModel* _battleModel;
	Unit* _unit;
	float _routingTimer;

public:
	UnitCounter(BattleModel* battleModel, Unit* unit);
	~UnitCounter();

	bool Animate(float seconds);
};


#endif
