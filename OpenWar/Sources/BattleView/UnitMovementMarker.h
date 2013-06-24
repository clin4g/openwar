// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMovementMarker_H
#define UnitMovementMarker_H

#include "UnitMarker.h"

class ColorBillboardRenderer;
class TextureBillboardRenderer;



class UnitMovementMarker : public UnitMarker
{
public:
	UnitMovementMarker(BattleModel* battleModel, Unit* unit);
	~UnitMovementMarker();

	bool Animate(float seconds);

	void RenderMovementFighters(ColorBillboardRenderer* renderer);
	void RenderMovementMarker(TextureBillboardRenderer* renderer);
	void RenderMovementPath(TextureTriangleRenderer* renderer);
};


#endif
