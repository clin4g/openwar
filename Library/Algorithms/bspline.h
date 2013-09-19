// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BSPLINE_H
#define BSPLINE_H

#include <vector>
#include <glm/glm.hpp>


extern const glm::mat4x4 bspline_matrix;
extern const glm::mat4x4 bspline_matrix_transpose;
extern const glm::mat4x4 bspline_split_right;
extern const glm::mat4x4 bspline_split_left;


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


void bspline_join(std::vector<glm::vec2>& path, float tolerance);
void bspline_split(std::vector<glm::vec2>& path, float tolerance);

std::vector<std::pair<glm::vec2, glm::vec2>> spline_line_strip(const std::vector<glm::vec2>& path);
std::vector<glm::vec2> bspline_offset(std::vector<std::pair<glm::vec2, glm::vec2>>& strip, float offset);


#endif
