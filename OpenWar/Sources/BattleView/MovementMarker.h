// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef MovementMarker_H
#define MovementMarker_H

#include "BattleModel.h"


class MovementMarker
{
public:
	BattleModel* _battleModel;
	Unit* _unit;

public:
	MovementMarker(BattleModel* battleModel, Unit* unit);
	~MovementMarker();

	bool Animate(float seconds);
};


#endif
