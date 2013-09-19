// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorBillboardRenderer_H
#define ColorBillboardRenderer_H

#include "../Graphics/renderer.h"


class ColorBillboardRenderer
{
	struct vertex
	{
		glm::vec3 _position;
		glm::vec4 _color;
		float _height;

		vertex(glm::vec3 p, glm::vec4 c, float h) : _position(p), _color(c), _height(h)
		{
		}
	};

	struct uniforms
	{
		glm::mat4x4 _transform;
		glm::vec3 _upvector;
		float _viewport_height;
	};

	renderer<vertex, uniforms>* _renderer;
	vertexbuffer<vertex> _vbo;

public:
	ColorBillboardRenderer();
	~ColorBillboardRenderer();

	void Reset();
	void AddBillboard(const glm::vec3& position, const glm::vec4& color, float height);
	void Draw(const glm::mat4x4& transform, const glm::vec3 cameraUp, float viewportHeight);
};


#endif
