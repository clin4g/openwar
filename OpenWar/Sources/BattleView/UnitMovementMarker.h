// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMovementMarker_H
#define UnitMovementMarker_H

#include "UnitMarker.h"

class BattleView;
class ColorBillboardRenderer;
class TextureBillboardRenderer;
class TextureTriangleRenderer;


class UnitMovementMarker : public UnitMarker
{
public:
	UnitMovementMarker(BattleModel* battleModel, Unit* unit);
	~UnitMovementMarker();

	bool Animate(float seconds);

	void RenderMovementMarker(TextureBillboardRenderer* renderer);
	void AppendFacingMarker(TextureTriangleRenderer* renderer, BattleView* battleView);
	void RenderMovementFighters(ColorBillboardRenderer* renderer);
	void RenderMovementPath(GradientTriangleRenderer* renderer);
};


#endif
