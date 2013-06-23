// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "PlainRenderer.h"


PlainRenderer::PlainRenderer()
{
	_renderer = new renderer<vertex, uniforms>((
		VERTEX_ATTRIBUTE(vertex, _position),
		SHADER_UNIFORM(uniforms, _transform),
		SHADER_UNIFORM(uniforms, _point_size),
		SHADER_UNIFORM(uniforms, _color),
		VERTEX_SHADER
		({
			attribute vec3 position;
			uniform mat4 transform;
			uniform float point_size;

			void main()
			{
				vec4 p = transform * vec4(position, 1);

				gl_Position = p;
				gl_PointSize = point_size;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform vec4 color;

			void main()
			{
				gl_FragColor = color;
			}
		}))
	);
	_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


PlainRenderer::~PlainRenderer()
{
}


void PlainRenderer::Reset()
{
	_vbo._vertices.clear();
}


void PlainRenderer::Draw(const glm::mat4x4& transform, const glm::vec4& color)
{
	glLineWidth(1);

	_vbo._mode = GL_LINES;

	uniforms uniforms;
	uniforms._transform = transform;
	uniforms._color = color;

	_renderer->render(_vbo, uniforms);
}


void PlainLineRenderer::AddLine(const glm::vec3& p1, const glm::vec3& p2)
{
	_vbo._vertices.push_back(vertex(p1));
	_vbo._vertices.push_back(vertex(p2));
}
