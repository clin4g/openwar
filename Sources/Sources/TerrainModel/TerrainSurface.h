// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TerrainSurface_H
#define TerrainSurface_H

#include "../../Library/Algebra/geometry.h"


enum class TerrainFeature { Hills, Water, Trees, Fords };


class TerrainSurface
{
public:
	TerrainSurface();
	virtual ~TerrainSurface();

	virtual bounds2f GetBounds() const = 0;
	virtual float GetHeight(glm::vec2 position) const = 0;
	virtual const float* Intersect(ray r) = 0;

	virtual bool IsForest(glm::vec2 position) const = 0;
	virtual bool IsImpassable(glm::vec2 position) const = 0;

	glm::vec3 GetPosition(glm::vec2 p, float h) { return glm::vec3(p, GetHeight(p) + h); }
};


#endif
