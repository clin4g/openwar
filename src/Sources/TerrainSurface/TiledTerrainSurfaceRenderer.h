// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TiledTerrainSurfaceRenderer_H
#define TiledTerrainSurfaceRenderer_H

#include "TerrainSurfaceRenderer.h"
#include "texture.h"

class TiledTerrainSurface;


class TiledTerrainSurfaceRenderer : public TerrainSurfaceRenderer
{
	TiledTerrainSurface* _terrainSurfaceModel;

public:
	TiledTerrainSurfaceRenderer(TiledTerrainSurface* terrainSurfaceModel);
	virtual ~TiledTerrainSurfaceRenderer();

	TiledTerrainSurface* GetTerrainSurfaceModel() const { return _terrainSurfaceModel; }

	virtual void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal);
};


#endif
