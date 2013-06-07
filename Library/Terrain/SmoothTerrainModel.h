/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef SMOOTHTERRAINMODEL_H
#define SMOOTHTERRAINMODEL_H

#include "bounds.h"
#include "geometry.h"
#include "matrix.h"

class image;


class SmoothTerrainModel
{
public:
	bounds2f _bounds;
	image* _map;
	matrix _heights;
	float _height;

public:
	SmoothTerrainModel(bounds2f bounds, image* map);
	~SmoothTerrainModel();

	matrix_size GetSize() const { return _heights.size(); }
	const bounds2f& GetBounds() const { return _bounds; }
	float GetMaxHeight() const { return _height; }

	float GetHeight(int x, int y) const;
	void SetHeight(int x, int y, float h);

	float GetHeight(glm::vec2 position) const;
	glm::vec3 GetNormal(glm::vec2 position) const;

	bool ContainsWater(bounds2f bounds) const;

	const float* Intersect(ray r);
};


#endif
