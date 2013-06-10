// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "bounds.h"
#include "vertex.h"

#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif


struct renderer_vertex_attribute;


class shape_base
{
public:
	GLenum _mode;
	GLuint _vbo;
	GLuint _vao;
	GLsizei _count;

	shape_base();
	shape_base(const shape_base& other);
	virtual ~shape_base();


	void _bind(const std::vector<renderer_vertex_attribute>& vertex_attributes, const void* data);
	void unbind(const std::vector<renderer_vertex_attribute>& vertex_attributes);

private:
	shape_base& operator=(const shape_base& other) { return *this; }
};



template <class _Vertex>
class shape : public shape_base
{
public:
	typedef _Vertex vertex_type;

	std::vector<vertex_type> _vertices;

	shape()
	{
	}

	shape(const shape& other) : shape_base(other), _vertices(other._vertices)
	{
	}

	GLsizei count() const
	{
		return _vbo != 0 ? _count : (GLsizei)_vertices.size();
	}

	virtual void update(GLenum usage)
	{
		if (_vbo == 0)
		{
			glGenBuffers(1, &_vbo);
			CHECK_ERROR_GL();
			if (_vbo == 0)
				return;
		}

		GLsizeiptr size = sizeof(vertex_type) * _vertices.size();
		const GLvoid* data = _vertices.data();

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		CHECK_ERROR_GL();
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
		CHECK_ERROR_GL();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_ERROR_GL();

		_count = (GLsizei)_vertices.size();
	}

	void bind(const std::vector<renderer_vertex_attribute>& vertex_attributes)
	{
		_bind(vertex_attributes, _vertices.data());
	}


private:
	shape& operator=(const shape&) { return *this; }
};



class plain_shape : public shape<plain_vertex>
{
public:
	void rectangle(bounds2f bounds);
};



class color_shape : public shape<color_vertex>
{
public:
	void add_line(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);
	void add_lines(bounds2f bounds, glm::vec4 color);

	void rectangle(bounds2f bounds);
};



class texture_shape : public shape<texture_vertex>
{
public:
	texture_shape();
	virtual ~texture_shape();

	void add_triangles(bounds2f vertex_bounds, bounds2f texture_bounds);
	void reshape(bounds2f vertex_bounds, bounds2f texture_bounds);
	void rectangle(bounds2f bounds);
};





#endif
