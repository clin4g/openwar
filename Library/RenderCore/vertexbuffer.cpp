// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "vertexbuffer.h"
#include "renderer.h"



vertexbuffer_base::vertexbuffer_base() :
_mode(0),
_vbo(0),
_vao(0),
_count(0)
{
}





vertexbuffer_base::~vertexbuffer_base()
{
	if (_vao != 0)
	{
		glDeleteVertexArraysOES(1, &_vao);
		CHECK_ERROR_GL();
	}
	if (_vbo != 0)
	{
		glDeleteBuffers(1, &_vbo);
		CHECK_ERROR_GL();
	}
}



void vertexbuffer_base::_bind(const std::vector<renderer_vertex_attribute>& vertex_attributes, const void* data)
{
	bool setup = _vao == 0;

	if (_vbo != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		CHECK_ERROR_GL();

		if (_vao == 0)
		{
			glGenVertexArraysOES(1, &_vao);
			CHECK_ERROR_GL();
		}
	}

	if (_vao != 0)
	{
		glBindVertexArrayOES(_vao);
		CHECK_ERROR_GL();
	}

	if (setup)
	{
		const char* ptr = _vbo != 0 ? nullptr : reinterpret_cast<const char*>(data);
		for (GLuint index = 0; index < vertex_attributes.size(); ++index)
		{
			glEnableVertexAttribArray(index);
			CHECK_ERROR_GL();

			const renderer_vertex_attribute& item = vertex_attributes[index];
			const GLvoid* offset = reinterpret_cast<const GLvoid*>(ptr + item._offset);

			glVertexAttribPointer(index, item._size, item._type, GL_FALSE, item._stride, offset);
			CHECK_ERROR_GL();
		}
	}
}



void vertexbuffer_base::unbind(const std::vector<renderer_vertex_attribute>& vertex_attributes)
{
	if (_vao != 0)
	{
		glBindVertexArrayOES(0);
		CHECK_ERROR_GL();
	}
	else
	{
		for (GLuint index = 0; index < vertex_attributes.size(); ++index)
		{
			glDisableVertexAttribArray(index);
			CHECK_ERROR_GL();
		}
	}

	if (_vbo)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_ERROR_GL();
	}
}
