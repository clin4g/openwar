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


class vertexbuffer_base
{
public:
	GLenum _mode;
	GLuint _vbo;
	GLuint _vao;
	GLsizei _count;

	vertexbuffer_base();
	virtual ~vertexbuffer_base();


	void _bind(const std::vector<renderer_vertex_attribute>& vertex_attributes, const void* data);
	void unbind(const std::vector<renderer_vertex_attribute>& vertex_attributes);

private:
	vertexbuffer_base(const vertexbuffer_base&) {}
	vertexbuffer_base& operator=(const vertexbuffer_base&) { return *this; }
};



template <class _Vertex>
class vertexbuffer : public vertexbuffer_base
{
public:
	typedef _Vertex vertex_type;

	std::vector<vertex_type> _vertices;

	vertexbuffer()
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
	vertexbuffer(const vertexbuffer& other) { }
	vertexbuffer& operator=(const vertexbuffer&) { return *this; }
};


#endif
