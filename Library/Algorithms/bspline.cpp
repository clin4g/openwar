/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "bspline.h"


float bspline_patch(const glm::mat4x4& p, const glm::vec2& t)
{
	static glm::mat4x4 _bspline_matrix(
			-1 / 6.0f, 3 / 6.0f, -3 / 6.0f, 1 / 6.0f,
			3 / 6.0f, -6 / 6.0f, 3 / 6.0f, 0,
			-3 / 6.0f, 0, 3 / 6.0f, 0,
			1 / 6.0f, 4 / 6.0f, 1 / 6.0f, 0
	);

	static glm::mat4x4 _bspline_matrix_transpose(
			-1 / 6.0f, 3 / 6.0f, -3 / 6.0f, 1 / 6.0f,
			3 / 6.0f, -6 / 6.0f, 0, 4 / 6.0f,
			-3 / 6.0f, 3 / 6.0f, 3 / 6.0f, 1 / 6.0f,
			1 / 6.0f, 0, 0, 0
	);

	glm::vec2 tt = t * t;
	glm::vec2 ttt = t * tt;
	glm::vec4 u = glm::vec4(ttt.x, tt.x, t.x, 1);
	glm::vec4 v = glm::vec4(ttt.y, tt.y, t.y, 1);

	return glm::dot(u, _bspline_matrix_transpose * p * _bspline_matrix * v);
}
