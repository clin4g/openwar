// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainSurface_H
#define SmoothTerrainSurface_H

#include "../../Library/Algebra/bounds.h"
#include "../../Library/Algorithms/heightmap.h"
#include "../TerrainModel/TerrainSurface.h"

class image;
class SmoothTerrainSurfaceRenderer;


class SmoothTerrainSurface : public TerrainSurface
{
public:
	bounds2f _bounds;
	image* _groundmap;
	heightmap _heightmap;
	glm::vec2 _scaleImageToWorld;

public:
	SmoothTerrainSurfaceRenderer* _renderer;

	SmoothTerrainSurface(bounds2f bounds, image* map);
	virtual ~SmoothTerrainSurface();

	virtual bounds2f GetBounds() const { return _bounds; }

	virtual const float* Intersect(ray r);

	virtual bool IsForest(glm::vec2 position) const;
	virtual bool IsImpassable(glm::vec2 position) const;

	image* GetGroundtMap() const { return _groundmap; }

	void LoadHeightmapFromImage();
	void SaveHeightmapToImage();

	virtual float GetHeight(glm::vec2 position) const;
	float CalculateHeight(glm::vec2 position) const;

	void Extract(glm::vec2 position, image* brush);
	bounds2f Paint(TerrainFeature feature, glm::vec2 position, image* img, float pressure);
	bounds2f Paint(TerrainFeature feature, glm::vec2 position, float radius, float pressure);

	glm::ivec2 MapWorldToImage(glm::vec2 position) const;
	glm::vec2 MapImageToWorld(glm::ivec2 p) const;
};


#endif
