// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BILLBOARDRENDERER_H
#define BILLBOARDRENDERER_H

#include "renderer.h"
#include "BillboardTexture.h"


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


struct Billboard
{
	glm::vec3 position;
	float height;
	int shape;

	Billboard() {}
	Billboard(glm::vec3 p, float h, int s) : position(p), height(h), shape(s) {}
};


struct BillboardModel
{
	BillboardTexture* texture;
	std::vector<Billboard> billboards;
};


class BillboardRenderer
{
public:
	renderer<texture_billboard_vertex, texture_billboard_uniforms>* _texture_billboard_renderer;
	vertexbuffer<texture_billboard_vertex> _vbo;

	BillboardRenderer();
	~BillboardRenderer();

	void Render(BillboardModel* billboardModel, const glm::mat4x4& transform, const glm::vec3& cameraUp, float cameraFacing);
};



#endif
