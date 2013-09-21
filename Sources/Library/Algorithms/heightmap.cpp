// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "heightmap.h"
#include "bspline.h"


heightmap::heightmap(glm::ivec2 size) :
_size(size),
_values(nullptr)
{
	_values = new float[size.x * size.y];
}


heightmap::~heightmap()
{
	delete [] _values;
}


float heightmap::get_height(int x, int y) const
{
	if (0 <= x && x < _size.x && 0 <= y && y < _size.y)
		return _values[x + _size.x * y];
	return 0;
}


void heightmap::set_height(int x, int y, float value)
{
	if (0 <= x && x < _size.x && 0 <= y && y < _size.y)
		_values[x + _size.x * y] = value;
}


float heightmap::interpolate(glm::vec2 position) const
{
	int x = (int)glm::floor(position.x);
	int y = (int)glm::floor(position.y);

	glm::mat4x4 p(
			get_height(x - 1, y - 1), get_height(x + 0, y - 1), get_height(x + 1, y - 1), get_height(x + 2, y - 1),
			get_height(x - 1, y + 0), get_height(x + 0, y + 0), get_height(x + 1, y + 0), get_height(x + 2, y + 0),
			get_height(x - 1, y + 1), get_height(x + 0, y + 1), get_height(x + 1, y + 1), get_height(x + 2, y + 1),
			get_height(x - 1, y + 2), get_height(x + 0, y + 2), get_height(x + 1, y + 2), get_height(x + 2, y + 2)
	);

	glm::vec2 t = position - glm::vec2(x, y);

	return bspline_interpolate(p, t);
}





static bool almost_zero(float value)
{
	static const float epsilon = 10 * std::numeric_limits<float>::epsilon();
	return fabsf(value) < epsilon;
}


const float* heightmap::intersect(ray r) const
{
	static float result;

	bounds1f height = bounds1f(-100, 1000); //min(m), max(m));
	bounds2f bounds(0, 0, _size.x - 1, _size.y - 1);
	bounds2f quad(-0.01f, -0.01f, 1.01f, 1.01f);

	const float* d = ::intersect(r, bounds3f(bounds, height));
	if (d == nullptr)
		return nullptr;

	glm::vec3 p = r.point(*d);

	bounds2f bounds_2(0, 0, _size.x - 2, _size.y - 2);

	int x = (int)bounds_2.x().clamp(p.x);
	int y = (int)bounds_2.y().clamp(p.y);
	int flipX = r.direction.x < 0 ? 0 : 1;
	int flipY = r.direction.y < 0 ? 0 : 1;
	int dx = r.direction.x < 0 ? -1 : 1;
	int dy = r.direction.y < 0 ? -1 : 1;

	while (height.contains(p.z) && bounds_2.contains(x, y))
	{
		glm::vec3 v1 = glm::vec3(x, y, get_height(x, y));
		glm::vec3 v2 = glm::vec3(x + 1, y, get_height(x + 1, y));
		glm::vec3 v3 = glm::vec3(x, y + 1, get_height(x, y + 1));
		glm::vec3 v4 = glm::vec3(x + 1, y + 1, get_height(x + 1, y + 1));

		d = ::intersect(r, plane(v2, v4, v3));
		if (d != nullptr)
		{
			glm::vec2 rel = (r.point(*d) - v1).xy();
			if (quad.contains(rel) && rel.x >= 1 - rel.y)
			{
				result = *d;
				return &result;
			}
		}

		d = ::intersect(r, plane(v1, v2, v3));
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
