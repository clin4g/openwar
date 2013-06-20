// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TERRAINSURFACEMODELSMOOTH_H
#define TERRAINSURFACEMODELSMOOTH_H

#include "bounds.h"
#include "heightmap.h"
#include "TerrainSurfaceModel.h"

class image;


class TerrainSurfaceModelSmooth : public TerrainSurfaceModel
{
public:
	bounds2f _bounds;
	image* _map;
	heightmap _heightmap;
	float _height;
	glm::vec2 _scaleWorldToImage;
	glm::vec2 _scaleImageToWorld;

public:
	TerrainSurfaceModelSmooth(bounds2f bounds, image* map);
	virtual ~TerrainSurfaceModelSmooth();

	virtual float GetHeight(glm::vec2 position) const;
	virtual glm::vec3 GetNormal(glm::vec2 position) const;
	virtual const float* Intersect(ray r);

	virtual bool IsWater(glm::vec2 position) const;
	virtual bool IsForest(glm::vec2 position) const;
	virtual bool IsImpassable(glm::vec2 position) const;

	virtual bool ContainsWater(bounds2f bounds) const;



	image* GetMap() const { return _map; }

	void LoadHeightmapFromImage();
	void SaveHeightmapToImage();

	const bounds2f& GetBounds() const { return _bounds; }
	float GetMaxHeight() const { return _height; }

	float GetHeight(int x, int y) const;

	bounds2f EditHills(glm::vec2 position, float radius, float pressure);
	bounds2f EditWater(glm::vec2 position, float radius, float pressure);
	bounds2f EditTrees(glm::vec2 position, float radius, float pressure);
};


#endif
