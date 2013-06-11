// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "geometry.h"


class heightmap
{
	glm::ivec2 _size;
	float* _values;

public:
	heightmap(glm::ivec2 size);
	~heightmap();

	glm::ivec2 size() const { return _size; }

	float get_height(int x, int y) const;
	void set_height(int x, int y, float value);

	float interpolate(glm::vec2 position) const;
	const float* intersect(ray r) const;
};


#endif
