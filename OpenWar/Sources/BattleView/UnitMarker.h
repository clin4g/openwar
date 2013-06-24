// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMarker_H
#define UnitMarker_H

class BattleModel;
class TextureTriangleRenderer;
class Unit;


class UnitMarker
{
protected:
	BattleModel* _battleModel;
	Unit* _unit;

public:
	UnitMarker(BattleModel* battleModel, Unit* unit);
	virtual ~UnitMarker();

	Unit* GetUnit() const { return _unit; }

public:
	void _Path(TextureTriangleRenderer* renderer, int mode, float scale, const std::vector<glm::vec2>& path, float t0);
	void Path(TextureTriangleRenderer* renderer, int mode, glm::vec2 position, const std::vector<glm::vec2>& path, float t0);
};



#endif
