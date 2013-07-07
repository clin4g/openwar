// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef KillZoneMarker_H
#define KillZoneMarker_H

#include "BattleModel.h"
class GradientTriangleStripRenderer;


class KillZoneMarker
{
public:
	BattleModel* _battleModel;
	Unit* _unit;

public:
	KillZoneMarker(BattleModel* battleModel, Unit* unit);

	void Render(GradientTriangleStripRenderer* renderer);

private:
	void RenderMeleeReach(GradientTriangleStripRenderer* renderer);
	void RenderMissileRange(GradientTriangleStripRenderer* renderer, glm::vec2 position, float direction, float minimumRange, float maximumRange);

	glm::vec3 GetPosition(glm::vec2 p) const;
};


#endif
