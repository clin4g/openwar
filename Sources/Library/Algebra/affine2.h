// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef AFFINE2_H
#define AFFINE2_H

#include <glm/glm.hpp>


struct affine2
{
	glm::mat2x2 mul;
	glm::vec2 add;

	affine2();
	affine2(glm::vec2 v0, glm::vec2 v1);

	glm::vec2 transform(glm::vec2 value) const;
};


inline affine2::affine2() : mul(), add()
{
}


inline affine2::affine2(glm::vec2 v0, glm::vec2 v1) :
mul(v1.x - v0.x, 0, 0, v1.y - v0.y),
add(v0)
{
}


inline glm::vec2 affine2::transform(glm::vec2 value) const
{
	return value * mul + add;
}


#endif
