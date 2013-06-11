// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BSPLINE_H
#define BSPLINE_H


glm::mat4x4 bspline_matrix_product(const glm::mat4x4& p);


inline glm::vec4 bspline_basis_vector(float t)
{
	float t2 = t * t;
	float t3 = t * t2;
	return glm::vec4(t3, t2, t, 1);
}


inline float bspline_interpolate(const glm::mat4x4& p, const glm::vec2& t)
{
	return glm::dot(bspline_basis_vector(t.x), bspline_matrix_product(p) * bspline_basis_vector(t.y));
}


#endif
