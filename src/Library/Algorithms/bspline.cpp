// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "bspline.h"
#include "../Algebra/geometry.h"


const glm::mat4x4 bspline_matrix(
	glm::mat4x4(
		-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 0, 3, 0,
		1, 4, 1, 0) / 6.0f
);


const glm::mat4x4 bspline_matrix_transpose(
	glm::mat4x4(
		-1, 3, -3, 1,
		3, -6, 0, 4,
		-3, 3, 3, 1,
		1, 0, 0, 0) / 6.0f
);


const glm::mat4x4 bspline_split_right(
	glm::mat4x4(
		0, 1, 6, 1,
		0, 4, 4, 0,
		1, 6, 1, 0,
		4, 4, 0, 0
	) / 8.0f
);


const glm::mat4x4 bspline_split_left(
	glm::mat4x4(
		0, 0, 4, 4,
		0, 1, 6, 1,
		0, 4, 4, 0,
		1, 6, 1, 0
	) / 8.0f
);


glm::mat4x4 bspline_matrix_product(const glm::mat4x4& p)
{
	return bspline_matrix_transpose * p * bspline_matrix;
}



static bool bspline_join(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float tolerance)
{
	glm::vec2 d1 = p1 - p0;
	glm::vec2 d2 = p2 - p0;
	glm::vec2 x2 = d2 * glm::length(d1) / glm::length(d2);
	return glm::distance(d1, x2) < tolerance;
}


void bspline_join(std::vector<glm::vec2>& path, float tolerance)
{
	std::vector<glm::vec2>::iterator i = path.begin() + 2;
	while (i < path.end())
	{
		if (bspline_join(*(i - 2), *(i - 1), *i, tolerance))
			i = path.erase(i - 1) + 1;
		else
			++i;
	}
}



static bool bspline_split(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float tolerance)
{
	return !bspline_join(p0, p1, p2, tolerance) || !bspline_join(p1, p2, p3, tolerance);
}


void bspline_split(std::vector<glm::vec2>& path, float tolerance)
{
	std::vector<glm::vec2>::iterator i = path.begin() + 3;
	while (i < path.end())
	{
		glm::vec2 p0 = *(i - 3);
		glm::vec2 p1 = *(i - 2);
		glm::vec2 p2 = *(i - 1);
		glm::vec2 p3 = *i;

		if (bspline_split(p0, p1, p2, p3, tolerance))
		{
			glm::vec4 px = glm::vec4(p0.x, p1.x, p2.x, p3.x);
			glm::vec4 py = glm::vec4(p0.y, p1.y, p2.y, p3.y);

			glm::vec4 plx = px * bspline_split_left;
			glm::vec4 ply = py * bspline_split_left;
			glm::vec4 prx = px * bspline_split_right;
			glm::vec4 pry = py * bspline_split_right;

			i = path.erase(i - 2, i);
			i = path.insert(i, glm::vec2(plx[0], ply[0]));
			i = path.insert(i, glm::vec2(plx[1], ply[1]));
			i = path.insert(i, glm::vec2(plx[2], ply[2]));
			i = path.insert(i, glm::vec2(prx[2], pry[2]));
			i = path.insert(i, glm::vec2(prx[3], pry[3]));
			i += 2;
		}
		else
		{
			++i;
		}
	}
}



std::vector<std::pair<glm::vec2, glm::vec2>> spline_line_strip(const std::vector<glm::vec2>& path)
{
	std::vector<std::pair<glm::vec2, glm::vec2>> result;
	if (path.size() >= 4)
	{
		for (std::vector<glm::vec2>::const_iterator i = path.begin() + 2; i < path.end(); ++i)
		{
			glm::vec2 p = (*(i - 2) + *(i - 1) * 4.0f + *i) / 6.0f;
			glm::vec2 v = 0.5f * (*i - *(i - 2));
			result.push_back(std::pair<glm::vec2, glm::vec2>(p, v));
		}
	}
	return result;
}



std::vector<glm::vec2> bspline_offset(std::vector<std::pair<glm::vec2, glm::vec2>>& strip, float offset)
{
	std::vector<glm::vec2> result;

	for (std::pair<glm::vec2, glm::vec2> i : strip)
		result.push_back(i.first + offset * glm::normalize(glm::vec2(-i.second.y, i.second.x)));

	return result;
}


