// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmoothTerrainWater.h"


SmoothTerrainWater::SmoothTerrainWater(bounds2f bounds, image* map) :
_map(map),
_bounds(bounds)
{
	_water_inside_renderer = new renderer<plain_vertex, ground_texture_uniforms>((
			VERTEX_ATTRIBUTE(plain_vertex, _position),
					SHADER_UNIFORM(ground_texture_uniforms, _transform),
					SHADER_UNIFORM(ground_texture_uniforms, _texture),
					VERTEX_SHADER
		({
						attribute
						vec2 position;
						uniform
						mat4 transform;

						void main()
						{
							vec4 p = transform * vec4(position, 0, 1);

							gl_Position = p;
							gl_PointSize = 1.0;
						}
					}),
					FRAGMENT_SHADER
		({
						void main()
						{
							gl_FragColor = vec4(0.44 * 0.5, 0.72 * 0.5, 0.91 * 0.5, 0.5);
						}
					})
	));
	_water_inside_renderer->_blend_sfactor = GL_ONE;
	_water_inside_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_water_border_renderer = new renderer<plain_vertex, ground_texture_uniforms>((
			VERTEX_ATTRIBUTE(plain_vertex, _position),
					SHADER_UNIFORM(ground_texture_uniforms, _transform),
					SHADER_UNIFORM(ground_texture_uniforms, _texture),
					VERTEX_SHADER
		({
						attribute
						vec2 position;
						uniform
						mat4 transform;
						varying
						vec2 _groundpos;

						void main()
						{
							vec4 p = transform * vec4(position, 0, 1);

							_groundpos = position;

							gl_Position = p;
							gl_PointSize = 1.0;
						}
					}),
					FRAGMENT_SHADER
		({
						varying
						vec2 _groundpos;

						void main()
						{
							if (distance(_groundpos, vec2(512.0, 512.0)) > 512.0)
								discard;

							gl_FragColor = vec4(0.44 * 0.5, 0.72 * 0.5, 0.91 * 0.5, 0.5);
						}
					})
	));
	_water_border_renderer->_blend_sfactor = GL_ONE;
	_water_border_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;

	Update();
}


SmoothTerrainWater::~SmoothTerrainWater()
{
}


bool SmoothTerrainWater::IsWater(glm::vec2 position) const
{
	int x = (int)(512 * position.x / 1024);
	int y = (int)(512 * position.y / 1024);
	glm::vec4 c = _map->get_pixel(x, y);
	return c.b >= 0.5;
}



bool SmoothTerrainWater::ContainsWater(bounds2f bounds) const
{
	glm::ivec2 size(512, 512);
	glm::vec2 min = glm::vec2(size.x - 1, size.y - 1) * (bounds.min - _bounds.min) / _bounds.size();
	glm::vec2 max = glm::vec2(size.x - 1, size.y - 1) * (bounds.max - _bounds.min) / _bounds.size();
	int xmin = (int)floorf(min.x);
	int ymin = (int)floorf(min.y);
	int xmax = (int)ceilf(max.x);
	int ymax = (int)ceilf(max.y);

	if (_map != nullptr)
	{
		for (int x = xmin; x <= xmax; ++x)
			for (int y = ymin; y <= ymax; ++y)
			{
				glm::vec4 c = _map->get_pixel(x, y);
				if (c.b >= 0.5 || c.r >= 0.5)
					return true;
			}
	}

	return false;
}



static int inside_circle(glm::vec2 p)
{
	return glm::length(p - glm::vec2(512, 512)) <= 512 ? 1 : 0;
}


static int inside_circle(plain_vertex v1, plain_vertex v2, plain_vertex v3)
{
	return inside_circle(v1._position)
			+ inside_circle(v2._position)
			+ inside_circle(v3._position);

}


static vertexbuffer<plain_vertex>* choose_shape(int count, vertexbuffer<plain_vertex>* inside, vertexbuffer<plain_vertex>* border)
{
	switch (count)
	{
		case 1:
		case 2:
			return border;

		case 3:
			return inside;

		default:
			return nullptr;
	}
}

void SmoothTerrainWater::Update()
{
	_shape_water_inside._mode = GL_TRIANGLES;
	_shape_water_border._mode = GL_TRIANGLES;

	_shape_water_inside._vertices.clear();
	_shape_water_border._vertices.clear();

	int n = 64;
	glm::vec2 s = glm::vec2(1024, 1024) / (float)n;
	for (int x = 0; x < n; ++x)
		for (int y = 0; y < n; ++y)
		{
			glm::vec2 p = s * glm::vec2(x, y);
			if (ContainsWater(bounds2f(p, p + s)))
			{
				plain_vertex v11 = plain_vertex(p);
				plain_vertex v12 = plain_vertex(p + glm::vec2(0, s.y));
				plain_vertex v21 = plain_vertex(p + glm::vec2(s.x, 0));
				plain_vertex v22 = plain_vertex(p + s);

				vertexbuffer<plain_vertex>* s = choose_shape(inside_circle(v11, v22, v12), &_shape_water_inside, &_shape_water_border);
				if (s != nullptr)
				{
					s->_vertices.push_back(v11);
					s->_vertices.push_back(v22);
					s->_vertices.push_back(v12);
				}

				s = choose_shape(inside_circle(v22, v11, v21), &_shape_water_inside, &_shape_water_border);
				if (s != nullptr)
				{
					s->_vertices.push_back(v22);
					s->_vertices.push_back(v11);
					s->_vertices.push_back(v21);
				}
			}
		}

	_shape_water_inside.update(GL_STATIC_DRAW);
	_shape_water_border.update(GL_STATIC_DRAW);
}


void SmoothTerrainWater::Render(const glm::mat4x4& transform)
{
	ground_texture_uniforms uniforms;
	uniforms._transform = transform;
	uniforms._texture = nullptr;

	_water_inside_renderer->render(_shape_water_inside, uniforms);
	_water_border_renderer->render(_shape_water_border, uniforms);
}
