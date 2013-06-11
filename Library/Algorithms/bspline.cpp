// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "bspline.h"


glm::mat4x4 bspline_matrix_product(const glm::mat4x4& p)
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

	return _bspline_matrix_transpose * p * _bspline_matrix;
}
