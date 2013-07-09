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

private:
	void RenderMissileRange(GradientTriangleStripRenderer* renderer, glm::vec2 position, float direction, float minimumRange, float maximumRange);
	void RenderMissileTarget(GradientTriangleStripRenderer* renderer, glm::vec2 target);

	glm::vec3 GetPosition(glm::vec2 p) const;
};


#endif
