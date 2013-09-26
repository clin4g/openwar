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
	GLushort _x, _y;
	glm::vec3 _position;
	glm::vec3 _normal;

	terrain_vertex(GLushort x, GLushort y, glm::vec3 p, glm::vec3 n) : _x(x), _y(y), _position(p), _normal(n) {}
};


struct skirt_vertex
{
	glm::vec3 _position;
	float _height;

	skirt_vertex() {}
	skirt_vertex(glm::vec3 p, float h) : _position(p), _height(h) { }
};


struct terrain_uniforms
{
	glm::mat4x4 _transform;
	glm::vec3 _light_normal;
	glm::vec4 _map_bounds;
	const texture* _colormap;
	const texture* _splatmap;
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
	texture* _colormap;
	texture* _splatmap;
	image* _splatmapImage;

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
	SmoothTerrainSurfaceRenderer(SmoothTerrainSurface* terrainSurfaceModel, bool render_edges);
	virtual ~SmoothTerrainSurfaceRenderer();

	SmoothTerrainSurface* GetTerrainSurfaceModel() const { return _terrainSurfaceModel; }
	bounds2f GetSourceBounds() const { return _terrainSurfaceModel->GetBounds(); }

	virtual void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal);

	void UpdateHeights();
	void UpdateNormals();

	float GetHeight(int x, int y) const { return _heights[x + y * _size]; }
	glm::vec3 GetNormal(int x, int y) const { return _normals[x + y * _size]; }

	float InterpolateHeight(glm::vec2 position) const;
	glm::vec3 InterpolateNormal(glm::vec2 position) const;

	void UpdateChanges(bounds2f bounds);
	void UpdateDepthTextureSize();
	void UpdateSplatmap();

	void InitializeShadow();
	void InitializeSkirt();
	void InitializeLines();

	vertexbuffer<terrain_vertex>* SelectTerrainVbo(int inside);

	void BuildTriangles();
	void PushTriangle(const bounds2f& bounds, const terrain_vertex& v0, const terrain_vertex& v1, const terrain_vertex& v2);
};


#endif
