// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RangeMarker_H
#define RangeMarker_H

#include "BattleModel.h"
class GradientTriangleStripRenderer;


class RangeMarker
{
public:
	BattleModel* _battleModel;
	Unit* _unit;

public:
	RangeMarker(BattleModel* battleModel, Unit* unit);

	void Render(GradientTriangleStripRenderer* renderer);
	void MakeRangeMarker(GradientTriangleStripRenderer* renderer, glm::vec2 position, float direction, float minimumRange, float maximumRange);
};


#endif
