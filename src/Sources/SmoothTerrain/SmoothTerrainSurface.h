// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainSurface_H
#define SmoothTerrainSurface_H

#include "../../Library/Algebra/bounds.h"
#include "../../Library/Algorithms/heightmap.h"
#include "../TerrainModel/TerrainSurface.h"

class image;


class SmoothTerrainSurface : public TerrainSurface
{
public:
	bounds2f _bounds;
	image* _map;
	heightmap _heightmap;
	float _height;
	glm::vec2 _scaleImageToWorld;

public:
	SmoothTerrainSurface(bounds2f bounds, image* map);
	virtual ~SmoothTerrainSurface();

	virtual bounds2f GetBounds() const { return _bounds; }
	float GetMaxHeight() const { return _height; }

	virtual float GetHeight(glm::vec2 position) const;
	virtual glm::vec3 GetNormal(glm::vec2 position) const;
	virtual const float* Intersect(ray r);

	virtual bool IsForest(glm::vec2 position) const;
	virtual bool IsImpassable(glm::vec2 position) const;

	image* GetMap() const { return _map; }

	void LoadHeightmapFromImage();
	void SaveHeightmapToImage();

	float GetHeight(int x, int y) const;

	void Extract(glm::vec2 position, image* brush);
	bounds2f Paint(TerrainFeature feature, glm::vec2 position, image* img, float pressure);
	bounds2f Paint(TerrainFeature feature, glm::vec2 position, float radius, float pressure);

private:
	glm::ivec2 MapCoord(glm::vec2 position) const;
};


#endif
