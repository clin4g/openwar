// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef MovementMarker_H
#define MovementMarker_H

class BattleModel;
class ColorBillboardRenderer;
class TextureBillboardRenderer;
class Unit;


class MovementMarker
{
public:
	BattleModel* _battleModel;
	Unit* _unit;

public:
	MovementMarker(BattleModel* battleModel, Unit* unit);
	~MovementMarker();

	bool Animate(float seconds);

	void RenderMovementFighters(ColorBillboardRenderer* renderer);
	void RenderMovementMarker(TextureBillboardRenderer* renderer);
};


#endif
