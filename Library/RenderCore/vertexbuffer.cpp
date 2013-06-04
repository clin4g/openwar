/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "vertexbuffer.h"
#include "renderer.h"



shape_base::shape_base() :
_mode(0),
_vbo(0),
_vao(0),
_count(0)
{
}



shape_base::shape_base(shape_base const & other) :
_mode(other._mode),
_vbo(0),
_vao(0),
_count(0)
{
}



/*shape_base& shape_base::operator=(const shape_base& other)
{
	return *this;
}*/



shape_base::~shape_base()
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



void shape_base::_bind(const std::vector<renderer_vertex_attribute>& vertex_attributes, const void* data)
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



void shape_base::unbind(const std::vector<renderer_vertex_attribute>& vertex_attributes)
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



void color_shape::rectangle(bounds2f bounds)
{
	_mode = GL_TRIANGLES;

	_vertices.clear();
	_vertices.push_back(color_vertex(bounds.p11(), glm::vec4(1, 0, 0, 1)));
	_vertices.push_back(color_vertex(bounds.p12(), glm::vec4(1, 1, 0, 1)));
	_vertices.push_back(color_vertex(bounds.p22(), glm::vec4(0, 1, 1, 1)));
	_vertices.push_back(color_vertex(bounds.p22(), glm::vec4(1, 1, 0, 1)));
	_vertices.push_back(color_vertex(bounds.p21(), glm::vec4(1, 0, 1, 1)));
	_vertices.push_back(color_vertex(bounds.p11(), glm::vec4(1, 0, 0, 1)));
}



void plain_shape::rectangle(bounds2f bounds)
{
	_mode = GL_TRIANGLES;

	_vertices.clear();
	_vertices.push_back(plain_vertex(bounds.p11()));
	_vertices.push_back(plain_vertex(bounds.p12()));
	_vertices.push_back(plain_vertex(bounds.p22()));
	_vertices.push_back(plain_vertex(bounds.p22()));
	_vertices.push_back(plain_vertex(bounds.p21()));
	_vertices.push_back(plain_vertex(bounds.p11()));
}



void color_shape::add_line(glm::vec2 p1, glm::vec2 p2, glm::vec4 color)
{
	_vertices.push_back(color_vertex(p1, color));
	_vertices.push_back(color_vertex(p2, color));
}



void color_shape::add_lines(bounds2f bounds, glm::vec4 color)
{
	add_line(bounds.p11(), bounds.p12(), color);
	add_line(bounds.p12(), bounds.p22(), color);
	add_line(bounds.p22(), bounds.p21(), color);
	add_line(bounds.p21(), bounds.p11(), color);
}


static texture_vertex make_texture_vertex(float x, float y, float tx, float ty)
{
	texture_vertex result;
	result._position = glm::vec2(x, y);
	result._texcoord = glm::vec2(tx, ty);
	return result;
}



texture_shape::texture_shape()
{
}



texture_shape::~texture_shape()
{
}



void texture_shape::add_triangles(bounds2f vertex_bounds, bounds2f texture_bounds)
{
	_vertices.push_back(make_texture_vertex(vertex_bounds.min.x, vertex_bounds.min.y, texture_bounds.min.x, texture_bounds.max.y));
	_vertices.push_back(make_texture_vertex(vertex_bounds.min.x, vertex_bounds.max.y, texture_bounds.min.x, texture_bounds.min.y));
	_vertices.push_back(make_texture_vertex(vertex_bounds.max.x, vertex_bounds.max.y, texture_bounds.max.x, texture_bounds.min.y));
	_vertices.push_back(make_texture_vertex(vertex_bounds.max.x, vertex_bounds.max.y, texture_bounds.max.x, texture_bounds.min.y));
	_vertices.push_back(make_texture_vertex(vertex_bounds.max.x, vertex_bounds.min.y, texture_bounds.max.x, texture_bounds.max.y));
	_vertices.push_back(make_texture_vertex(vertex_bounds.min.x, vertex_bounds.min.y, texture_bounds.min.x, texture_bounds.max.y));
}


void texture_shape::reshape(bounds2f vertex_bounds, bounds2f texture_bounds)
{
	_mode = GL_TRIANGLES;

	_vertices.clear();
	_vertices.push_back(make_texture_vertex(vertex_bounds.min.x, vertex_bounds.min.y, texture_bounds.min.x, texture_bounds.max.y));
	_vertices.push_back(make_texture_vertex(vertex_bounds.min.x, vertex_bounds.max.y, texture_bounds.min.x, texture_bounds.min.y));
	_vertices.push_back(make_texture_vertex(vertex_bounds.max.x, vertex_bounds.max.y, texture_bounds.max.x, texture_bounds.min.y));
	_vertices.push_back(make_texture_vertex(vertex_bounds.max.x, vertex_bounds.max.y, texture_bounds.max.x, texture_bounds.min.y));
	_vertices.push_back(make_texture_vertex(vertex_bounds.max.x, vertex_bounds.min.y, texture_bounds.max.x, texture_bounds.max.y));
	_vertices.push_back(make_texture_vertex(vertex_bounds.min.x, vertex_bounds.min.y, texture_bounds.min.x, texture_bounds.max.y));
}


void texture_shape::rectangle(bounds2f bounds)
{
	_mode = GL_TRIANGLES;

	_vertices.clear();
	_vertices.push_back(texture_vertex(bounds.p11(), glm::vec2(0, 1)));
	_vertices.push_back(texture_vertex(bounds.p12(), glm::vec2(0, 0)));
	_vertices.push_back(texture_vertex(bounds.p22(), glm::vec2(1, 0)));
	_vertices.push_back(texture_vertex(bounds.p22(), glm::vec2(1, 0)));
	_vertices.push_back(texture_vertex(bounds.p21(), glm::vec2(1, 1)));
	_vertices.push_back(texture_vertex(bounds.p11(), glm::vec2(0, 1)));
}
