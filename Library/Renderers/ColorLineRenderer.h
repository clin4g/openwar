// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorLineRenderer_H
#define ColorLineRenderer_H

#include "renderer.h"


class ColorLineRenderer
{
	struct color_vertex3
	{
		glm::vec3 _position;
		glm::vec4 _color;

		color_vertex3() {}
		color_vertex3(glm::vec3 p, glm::vec4 c) : _position(p), _color(c) {}
	};

	struct gradient_uniforms
	{
		glm::mat4x4 _transform;
		GLfloat _point_size;

		gradient_uniforms() : _point_size(1) {}
	};

	renderer<color_vertex3, gradient_uniforms>* _renderer;
	vertexbuffer<color_vertex3> _vbo;

public:
	ColorLineRenderer();
	~ColorLineRenderer();

	void Reset();
	void AddLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c1, const glm::vec4& c2);
	void Draw(const glm::mat4x4& transform);
};



#endif
