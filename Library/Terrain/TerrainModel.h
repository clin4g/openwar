// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TERRAINMODEL_H
#define TERRAINMODEL_H

#include "geometry.h"


class TerrainModel
{
public:
	TerrainModel();
	virtual ~TerrainModel();

	virtual float GetHeight(glm::vec2 position) const = 0;
	virtual glm::vec3 GetNormal(glm::vec2 position) const = 0;
	virtual const float* Intersect(ray r) = 0;

	virtual bool IsWater(glm::vec2 position) const = 0;
	virtual bool IsForest(glm::vec2 position) const = 0;
	virtual bool IsImpassable(glm::vec2 position) const = 0;

};



#endif
