// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ColorLineRenderer.h"


ColorLineRenderer::ColorLineRenderer()
{
	_renderer = new renderer<color_vertex3, gradient_uniforms>((
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


ColorLineRenderer::~ColorLineRenderer()
{
}


void ColorLineRenderer::Reset()
{
	_vbo._vertices.clear();
}


void ColorLineRenderer::AddLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c1, const glm::vec4& c2)
{
	_vbo._vertices.push_back(color_vertex3(p1, c1));
	_vbo._vertices.push_back(color_vertex3(p2, c2));
}


void ColorLineRenderer::Draw(const glm::mat4x4& transform)
{
	glLineWidth(1);

	_vbo._mode = GL_LINES;

	gradient_uniforms uniforms;
	uniforms._transform = transform;
	_renderer->render(_vbo, uniforms);
}
