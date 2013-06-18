// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TILEDTERRAINSURFACERENDERER_H
#define TILEDTERRAINSURFACERENDERER_H

#include "TerrainSurfaceRenderer.h"
#include "texture.h"

class TerrainSurfaceModelTiled;


class TerrainSurfaceRendererTiled : public TerrainSurfaceRenderer
{
	TerrainSurfaceModelTiled* _terrainSurfaceModel;

public:
	TerrainSurfaceRendererTiled(TerrainSurfaceModelTiled* terrainSurfaceModel);
	virtual ~TerrainSurfaceRendererTiled();

	TerrainSurfaceModelTiled* GetTerrainSurfaceModel() const { return _terrainSurfaceModel; }

	virtual void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal);
};


#endif
