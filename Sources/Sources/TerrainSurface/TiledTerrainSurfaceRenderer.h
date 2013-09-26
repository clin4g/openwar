// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TiledTerrainSurfaceRenderer_H
#define TiledTerrainSurfaceRenderer_H

#include <glm/glm.hpp>
#include "../../Library/Graphics/texture.h"

class TiledTerrainSurface;


class TiledTerrainSurfaceRenderer
{
	TiledTerrainSurface* _terrainSurfaceModel;

public:
	TiledTerrainSurfaceRenderer(TiledTerrainSurface* terrainSurfaceModel);
	virtual ~TiledTerrainSurfaceRenderer();

	TiledTerrainSurface* GetTerrainSurfaceModel() const { return _terrainSurfaceModel; }

	void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal);
};


#endif
