// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainSurfaceRenderer_H
#define SmoothTerrainSurfaceRenderer_H

#include <map>
#include <functional>

#include "SmoothTerrainSurface.h"
#include "../../Library/Graphics/framebuffer.h"
#include "../../Library/Graphics/renderbuffer.h"
#include "../../Library/Graphics/vertexbuffer.h"
#include "../../Library/Graphics/renderer.h"
#include "../TerrainSurface/TerrainSurfaceRenderer.h"


struct terrain_renderers;

struct terrain_vertex
{
	glm::vec3 _position;
	glm::vec3 _normal;

	terrain_vertex(glm::vec3 p, glm::vec3 n) : _position(p), _normal(n) {}
};


struct terrain_skirt_vertex
{
	glm::vec3 _position;
	float _height;

	terrain_skirt_vertex() {}
	terrain_skirt_vertex(glm::vec3 p, float h) : _position(p), _height(h) { }
};


struct terrain_uniforms
{
	glm::mat4x4 _transform;
	glm::vec3 _light_normal;
	glm::vec4 _map_bounds;
	const texture* _colors;
	const texture* _map;
};



struct sobel_uniforms
{
	glm::mat4x4 _transform;
	const texture* _depth;
};



class SmoothTerrainSurfaceRenderer : public TerrainSurfaceRenderer
{
	SmoothTerrainSurface* _terrainSurfaceModel;

	int _framebuffer_width;
	int _framebuffer_height;
	framebuffer* _framebuffer;
	renderbuffer* _colorbuffer;
	texture* _depth;
	texture* _colors;
	texture* _mapTexture;
	image* _mapImage;

	vertexbuffer<color_vertex3> _vboLines;
	vertexbuffer<terrain_vertex> _vboInside;
	vertexbuffer<terrain_vertex> _vboBorder;
	vertexbuffer<terrain_skirt_vertex> _vboSkirt;
	terrain_renderers* _renderers;

	renderer<plain_vertex, terrain_uniforms>* _ground_shadow_renderer;
	vertexbuffer<plain_vertex> _vboTerrainShadow;

public:
	SmoothTerrainSurfaceRenderer(SmoothTerrainSurface* terrainSurfaceModel, bool render_edges);
	virtual ~SmoothTerrainSurfaceRenderer();

	SmoothTerrainSurface* GetTerrainSurfaceModel() const { return _terrainSurfaceModel; }

	void InitializeTerrainShadow(bounds2f bounds);

	void UpdateHeights(bounds2f bounds);
	void UpdateMapTexture();

	virtual void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal);

private:
	vertexbuffer<terrain_vertex>* triangle_shape(int inside);

	void UpdateDepthTextureSize();
	void InitializeEdge();

	bounds3f GetBounds() const;

	void BuildLines(vertexbuffer<color_vertex3>& shape);
	void BuildTriangles();

	terrain_vertex MakeTerrainVertex(float x, float y);
};


#endif
