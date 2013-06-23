// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "GradientRenderer.h"


GradientRenderer::GradientRenderer()
{
	_renderer = new renderer<vertex, uniforms>((
			VERTEX_ATTRIBUTE(color_vertex3, _position),
			VERTEX_ATTRIBUTE(color_vertex3, _color),
			SHADER_UNIFORM(gradient_uniforms, _transform),
			SHADER_UNIFORM(gradient_uniforms, _point_size),
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


void GradientRenderer::Reset()
{
	_vbo._vertices.clear();
}


void GradientRenderer::Draw(const glm::mat4x4& transform)
{
	glLineWidth(1);

	_vbo._mode = GL_LINES;

	uniforms uniforms;
	uniforms._transform = transform;
	_renderer->render(_vbo, uniforms);
}


void GradientLineRenderer::AddLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c1, const glm::vec4& c2)
{
	_vbo._vertices.push_back(vertex(p1, c1));
	_vbo._vertices.push_back(vertex(p2, c2));
}
