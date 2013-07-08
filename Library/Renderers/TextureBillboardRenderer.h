// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TextureBillboardRenderer_H
#define TextureBillboardRenderer_H

#include "renderer.h"
#include "BillboardTexture.h"


struct Billboard
{
	glm::vec3 position;
	float facing;
	float height;
	int shape;

	Billboard() {}
	Billboard(glm::vec3 p, float f, float h, int s) : position(p), facing(f), height(h), shape(s) {}
};


struct BillboardModel
{
	BillboardTexture* texture;
	std::vector<Billboard> staticBillboards;
	std::vector<Billboard> dynamicBillboards;

	int _billboardTreeShapes[16];
	int _billboardShapeCasualtyAsh[8];
	int _billboardShapeCasualtySam[8];
	int _billboardShapeCasualtyCav[16];
	int _billboardShapeFighterSamBlue;
	int _billboardShapeFighterSamRed;
	int _billboardShapeFighterAshBlue;
	int _billboardShapeFighterAshRed;
	int _billboardShapeFighterCavBlue;
	int _billboardShapeFighterCavRed;
	int _billboardShapeSmoke[8];
};


struct texture_billboard_vertex
{
	glm::vec3 _position;
	float _height;
	float _order;
	glm::vec2 _texcoord;
	glm::vec2 _texsize;

	texture_billboard_vertex(glm::vec3 p, float h, glm::vec2 tc, glm::vec2 ts) : _position(p), _height(h), _texcoord(tc), _texsize(ts)
	{
	}
};

struct texture_billboard_uniforms
{
	glm::mat4x4 _transform;
	const texture* _texture;
	glm::vec3 _upvector;
	float _viewport_height;
	float _min_point_size;
	float _max_point_size;
};


class TextureBillboardRenderer
{
public:
	renderer<texture_billboard_vertex, texture_billboard_uniforms>* _texture_billboard_renderer;
	vertexbuffer<texture_billboard_vertex> _vbo;

public:
	TextureBillboardRenderer();
	~TextureBillboardRenderer();

	void Reset();
	void AddBillboard(glm::vec3 position, float height, affine2 texcoords);
	void Draw(texture* tex, const glm::mat4x4& transform, const glm::vec3& cameraUp, float cameraFacingDegrees, float viewportHeight, bounds1f sizeLimit = bounds1f(0, 1024));

	void Render(BillboardModel* billboardModel, const glm::mat4x4& transform, const glm::vec3& cameraUp, float viewportHeight, float cameraFacingDegrees);
};



#endif
