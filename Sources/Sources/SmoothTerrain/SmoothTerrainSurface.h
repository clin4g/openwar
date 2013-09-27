// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainSurface_H
#define SmoothTerrainSurface_H

#include "../../Library/Algebra/bounds.h"
#include "../TerrainModel/TerrainSurface.h"
#include "SmoothTerrainSurfaceRenderer.h"

class image;


class SmoothTerrainSurface : public TerrainSurface
{
	bounds2f _bounds;
	image* _groundmap;

	int _framebuffer_width;
	int _framebuffer_height;
	framebuffer* _framebuffer;
	renderbuffer* _colorbuffer;
	texture* _depth;
	texture* _colormap;
	texture* _splatmap;

	terrain_renderers* _renderers;
	vertexbuffer<plain_vertex> _vboShadow;
	vertexbuffer<terrain_vertex> _vboInside;
	vertexbuffer<terrain_vertex> _vboBorder;
	vertexbuffer<skirt_vertex> _vboSkirt;
	vertexbuffer<color_vertex3> _vboLines;

	int _size;
	float* _heights;
	glm::vec3* _normals;

public:
	SmoothTerrainSurface(bounds2f bounds, image* groundmap);
	virtual ~SmoothTerrainSurface();

	//
	// TerrainSurface
	//

	virtual bounds2f GetBounds() const { return _bounds; }
	virtual float GetHeight(glm::vec2 position) const;
	virtual const float* Intersect(ray r);

	virtual bool IsForest(glm::vec2 position) const;
	virtual bool IsImpassable(glm::vec2 position) const;


	void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal);


	//
	// SmoothTerrainSurface
	//

	image* GetGroundMap() const { return _groundmap; }

	void Extract(glm::vec2 position, image* brush);
	bounds2f Paint(TerrainFeature feature, glm::vec2 position, image* brush, float pressure);
	bounds2f Paint(TerrainFeature feature, glm::vec2 position, float radius, float pressure);

	void EnableRenderEdges();

	void UpdateHeights();
	float CalculateHeight(int x, int y) const;
	void UpdateNormals();

	float GetHeight(int x, int y) const { return _heights[x + y * _size]; }
	glm::vec3 GetNormal(int x, int y) const { return _normals[x + y * _size]; }

	float InterpolateHeight(glm::vec2 position) const;

	const float* InternalIntersect(ray r);

	void UpdateChanges(bounds2f bounds);
	void UpdateDepthTextureSize();
	void UpdateSplatmap();

	float GetForestValue(int x, int y) const;
	float GetImpassableValue(int x, int y) const;

	void InitializeShadow();
	void InitializeSkirt();
	void InitializeLines();

	vertexbuffer<terrain_vertex>* SelectTerrainVbo(int inside);

	void BuildTriangles();
	void PushTriangle(const terrain_vertex& v0, const terrain_vertex& v1, const terrain_vertex& v2);

	glm::ivec2 MapWorldToImage(glm::vec2 position) const;
};


#endif
