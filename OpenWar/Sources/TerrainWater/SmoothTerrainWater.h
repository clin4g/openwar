// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainWater_H
#define SmoothTerrainWater_H

#include "TerrainWater.h"
#include "renderer.h"
#include "image.h"


class SmoothTerrainWater : public TerrainWater
{
	struct ground_texture_uniforms
	{
		glm::mat4x4 _transform;
		const texture* _texture;
	};

	renderer<plain_vertex, ground_texture_uniforms>* _water_inside_renderer;
	renderer<plain_vertex, ground_texture_uniforms>* _water_border_renderer;

	vertexbuffer<plain_vertex> _shape_water_inside;
	vertexbuffer<plain_vertex> _shape_water_border;

	image* _map;
	bool _editor;
	bounds2f _bounds;

public:
	SmoothTerrainWater(image* map, bool editor);
	virtual ~SmoothTerrainWater();

	virtual bool IsWater(glm::vec2 position) const;
	virtual bool ContainsWater(bounds2f bounds) const;

	void Update();
	void Render(const glm::mat4x4& transform);
};


#endif
