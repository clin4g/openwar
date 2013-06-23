// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorLineRenderer_H
#define ColorLineRenderer_H

#include "renderer.h"


class GradientRenderer
{
protected:
	struct vertex
	{
		glm::vec3 _position;
		glm::vec4 _color;

		vertex() {}
		vertex(glm::vec3 p, glm::vec4 c) : _position(p), _color(c) {}
	};

	struct uniforms
	{
		glm::mat4x4 _transform;
		GLfloat _point_size;

		uniforms() : _point_size(1) {}
	};

	renderer<vertex, uniforms>* _renderer;
	vertexbuffer<vertex> _vbo;

public:
	GradientRenderer();
	~GradientRenderer();

	void Reset();
	void Draw(const glm::mat4x4& transform);
};


class GradientLineRenderer : public GradientRenderer
{
public:
	void AddLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c1, const glm::vec4& c2);
};


#endif
