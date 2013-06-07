/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "heightmap.h"
#include "bspline.h"
#include "image.h"



static bool almost_zero(float value)
{
	static const float epsilon = 10 * std::numeric_limits<float>::epsilon();
	return fabsf(value) < epsilon;
}



heightmap::heightmap(bounds2f bounds, image* map) :
_heights(128, 128),
_bounds(bounds),
_height(124.5),
_map(map)
{
	for (int x = 0; x < 128; ++x)
	{
		int xx = (int)(x * (double)map->_width / 128.0);
		for (int y = 0; y < 128; ++y)
		{
			int yy = (int)(y * (double)map->_height / 128.0);
			glm::vec4 c = map->get_pixel(xx, yy);
			set_height(x, y, 0.5f + 124.5f * c.a);
		}
	}
}


heightmap::~heightmap()
{
}


float heightmap::get_height(int x, int y) const
{
	matrix_size size = _heights.size();
	if (0 <= x && x < size.n && 0 <= y && y < size.m)
		return _heights(x, y);
	return 0;
}


void heightmap::set_height(int x, int y, float h)
{
	matrix_size size = _heights.size();
	if (0 <= x && x < size.n && 0 <= y && y < size.m)
		_heights(x, y) = h;
}


float heightmap::get_height(glm::vec2 position) const
{
	matrix_size size = _heights.size();
	glm::vec2 c = glm::vec2(size.n - 1, size.m - 1) * (position - _bounds.p11()) / _bounds.size();
	int x = (int)floorf(c.x);
	int y = (int)floorf(c.y);

	glm::mat4x4 p;
	glm::mat4x4::value_type* pp = glm::value_ptr(p);
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			pp[j + 4 * i] = get_height(x + j - 1, y + i - 1);

	glm::vec2 t = c - glm::vec2(x, y);

	float height = bspline_patch(p, t);

	if (_map != nullptr)
	{
		glm::vec4 color = _map->get_pixel((int)(position.x * 512.0 / 1024.0), (int)(position.y * 512.0 / 1024.0));

		float water = color.b;
		height = glm::mix(height, -2.5f, glm::step(0.5f, water));

		float fords = color.r;
		height = glm::mix(height, -0.5f, glm::step(0.5f, fords));
	}

	return height;
}


glm::vec3 heightmap::get_normal(glm::vec2 position) const
{
	float h = get_height(position);
	glm::vec3 v1 = glm::vec3(1, 0, get_height(position + glm::vec2(1, 0)) - h);
	glm::vec3 v2 = glm::vec3(0, 1, get_height(position + glm::vec2(0, 1)) - h);
	return glm::normalize(glm::cross(v1, v2));
}


bool heightmap::contains_water(bounds2f bounds) const
{
	matrix_size size(512, 512);
	glm::vec2 min = glm::vec2(size.n - 1, size.m - 1) * (bounds.min - _bounds.min) / _bounds.size();
	glm::vec2 max = glm::vec2(size.n - 1, size.m - 1) * (bounds.max - _bounds.min) / _bounds.size();
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


static float* intersect(ray r, const matrix& m)
{
	static float result;

	matrix_size size = m.size();

	bounds1f height = bounds1f(-100, 1000); //min(m), max(m));
	bounds2f bounds(0, 0, size.m - 1, size.n - 1);
	bounds2f quad(-0.01f, -0.01f, 1.01f, 1.01f);

	const float* d = intersect(r, bounds3f(bounds, height));
	if (d == nullptr)
		return nullptr;

	glm::vec3 p = r.point(*d);

	bounds2f bounds_2(0, 0, size.m - 2, size.n - 2);

	int x = (int)bounds_2.x().clamp(p.x);
	int y = (int)bounds_2.y().clamp(p.y);
	int flipX = r.direction.x < 0 ? 0 : 1;
	int flipY = r.direction.y < 0 ? 0 : 1;
	int dx = r.direction.x < 0 ? -1 : 1;
	int dy = r.direction.y < 0 ? -1 : 1;

	while (height.contains(p.z) && bounds_2.contains(x, y))
	{
		glm::vec3 v1 = glm::vec3(x, y, m(x, y));
		glm::vec3 v2 = glm::vec3(x + 1, y, m(x + 1, y));
		glm::vec3 v3 = glm::vec3(x, y + 1, m(x, y + 1));
		glm::vec3 v4 = glm::vec3(x + 1, y + 1, m(x + 1, y + 1));

		d = intersect(r, plane(v2, v4, v3));
		if (d != nullptr)
		{
			glm::vec2 rel = (r.point(*d) - v1).xy();
			if (quad.contains(rel) && rel.x >= 1 - rel.y)
			{
				result = *d;
				return &result;
			}
		}

		d = intersect(r, plane(v1, v2, v3));
		if (d != nullptr)
		{
			glm::vec2 rel = (r.point(*d) - v1).xy();
			if (quad.contains(rel) && rel.x <= 1 - rel.y)
			{
				result = *d;
				return &result;
			}
		}

		float xDist = almost_zero(r.direction.x) ? std::numeric_limits<float>::max() : (x - p.x + flipX) / r.direction.x;
		float yDist = almost_zero(r.direction.y) ? std::numeric_limits<float>::max() : (y - p.y + flipY) / r.direction.y;

		if (xDist < yDist)
		{
			x += dx;
			p += r.direction * xDist;
		}
		else
		{
			y += dy;
			p += r.direction * yDist;
		}
	}

	return nullptr;
}


const float* intersect(ray r, const heightmap& h)
{
	matrix_size size = h.size();
	bounds2f bounds = h.get_bounds();
	glm::vec3 offset = glm::vec3(bounds.min, 0);
	glm::vec3 scale = glm::vec3(glm::vec2(size.m - 1, size.n - 1) / bounds.size(), 1);

	ray r2 = ray(scale * (r.origin - offset), glm::normalize(scale * r.direction));
	const float* d = intersect(r2, h.heights());
	if (d == nullptr)
		return nullptr;

	static float result;
	result = glm::length((r2.point(*d) - r2.origin) / scale);
	return &result;
}
