/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "bounds.h"
#include "geometry.h"
#include "matrix.h"

class image;


class heightmap
{
public:
	matrix _heights;
	bounds2f _bounds;
	float _height;
	image* _water;
	image* _fords;

public:
	heightmap(bounds2f bounds, const image& height, image* water, image* fords);
	heightmap(bounds2f bounds, float height);
	~heightmap();

	matrix& heights() { return _heights; }
	const matrix& heights() const { return _heights; }

	matrix_size size() const { return _heights.size(); }
	const bounds2f& get_bounds() const { return _bounds; }
	float max_height() const { return _height; }

	float get_height(int x, int y) const;
	void set_height(int x, int y, float h);

	float get_height(glm::vec2 position) const;
	glm::vec3 get_normal(glm::vec2 position) const;

	bool contains_water(bounds2f bounds) const;
};

const float* intersect(ray r, const heightmap& h);


#endif
