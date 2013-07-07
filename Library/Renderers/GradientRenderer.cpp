// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "GradientRenderer.h"


GradientRenderer::GradientRenderer()
{
	_renderer = new renderer<vertex, uniforms>((
			VERTEX_ATTRIBUTE(vertex, _position),
			VERTEX_ATTRIBUTE(vertex, _color),
			SHADER_UNIFORM(uniforms, _transform),
			SHADER_UNIFORM(uniforms, _point_size),
			VERTEX_SHADER
		({
			attribute vec3 position;
			attribute vec4 color;
			uniform mat4 transform;
			uniform float point_size;
			varying vec4 v_color;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, position.z, 1);

				gl_Position = p;
				gl_PointSize = point_size;
				v_color = color;
			}
		}),
		FRAGMENT_SHADER
		({
			varying vec4 v_color;

			void main()
			{
				gl_FragColor = v_color;
			}
		})
	));
	_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


GradientRenderer::~GradientRenderer()
{
}


void GradientRenderer::Draw(const glm::mat4x4& transform)
{
	glLineWidth(1);

	uniforms uniforms;
	uniforms._transform = transform;
	_renderer->render(_vbo, uniforms);
}


/***/


void GradientLineRenderer::Reset()
{
	_vbo._mode = GL_LINES;
	_vbo._vertices.clear();
}


void GradientLineRenderer::AddLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c1, const glm::vec4& c2)
{
	_vbo._vertices.push_back(vertex(p1, c1));
	_vbo._vertices.push_back(vertex(p2, c2));
}


/***/


void GradientTriangleRenderer::Reset()
{
	_vbo._mode = GL_TRIANGLES;
	_vbo._vertices.clear();
}


void GradientTriangleRenderer::AddVertex(const glm::vec3& p, const glm::vec4& c)
{
	_vbo._vertices.push_back(vertex(p, c));
}


/***/


void GradientTriangleStripRenderer::Reset()
{
	_vbo._mode = GL_TRIANGLE_STRIP;
	_vbo._vertices.clear();
}


void GradientTriangleStripRenderer::AddVertex(const glm::vec3& p, const glm::vec4& c, bool separator)
{
	if (separator && !_vbo._vertices.empty())
	{
		_vbo._vertices.push_back(_vbo._vertices.back());
		_vbo._vertices.push_back(vertex(p, c));
	}

	_vbo._vertices.push_back(vertex(p, c));
}
