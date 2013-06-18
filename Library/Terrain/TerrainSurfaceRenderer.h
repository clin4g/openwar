// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TERRAINSURFACERENDERER_H
#define TERRAINSURFACERENDERER_H



class TerrainSurfaceRenderer
{
public:
	TerrainSurfaceRenderer();
	virtual ~TerrainSurfaceRenderer();

	virtual void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal) = 0;
};



#endif
