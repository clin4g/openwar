// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMarker_H
#define UnitMarker_H

class BattleModel;
class GradientLineRenderer;
class PlainTriangleRenderer;
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
	void RenderPath(GradientLineRenderer* renderer, const std::vector<glm::vec2>& path);
	//void _PathLines2(GradientLineRenderer* renderer, const std::vector<glm::vec2>& path, const std::vector<glm::vec2>& original);

	void Path(GradientLineRenderer* renderer, int mode, const std::vector<glm::vec2>& path);
};



#endif
