// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLERENDERING_H
#define BATTLERENDERING_H

#include "renderer.h"
#include "BillboardRenderer.h"


struct BattleRendering
{
	struct color_billboard_vertex
	{
		glm::vec3 _position;
		glm::vec4 _color;
		float _height;

		color_billboard_vertex(glm::vec3 p, glm::vec4 c, float h) : _position(p), _color(c), _height(h)
		{
		}
	};

	struct color_billboard_uniforms
	{
		glm::mat4x4 _transform;
		glm::vec3 _upvector;
		float _viewport_height;
	};

	struct ground_color_uniforms
	{
		glm::mat4x4 _transform;
		GLfloat _point_size;
		glm::vec4 _color;

		ground_color_uniforms() : _point_size(1) { }
	};

	struct ground_gradient_uniforms
	{
		glm::mat4x4 _transform;
		GLfloat _point_size;

		ground_gradient_uniforms() : _point_size(1) { }
	};

	struct ground_texture_uniforms
	{
		glm::mat4x4 _transform;
		const texture* _texture;
	};


	renderer<color_billboard_vertex, color_billboard_uniforms>* _color_billboard_renderer;

	renderer<color_vertex3, ground_gradient_uniforms>* _ground_gradient_renderer;
	renderer<plain_vertex3, ground_color_uniforms>* _ground_plain_renderer;
	renderer<texture_vertex3, ground_texture_uniforms>* _ground_texture_renderer;
	renderer<plain_vertex, plain_uniforms>* _ground_shadow_renderer;

	renderer<plain_vertex, ground_texture_uniforms>* _water_inside_renderer;
	renderer<plain_vertex, ground_texture_uniforms>* _water_border_renderer;

	texture* _textureBackgroundLinen;
	texture* _textureUnitMarkers;
	texture* _textureMovementBlue;
	texture* _textureMovementGray;
	texture* _textureMovementRed;
	texture* _textureMissileBlue;
	texture* _textureMissileGray;
	texture* _textureMissileRed;
	texture* _textureBillboards;
	texture* _textureTouchMarker;

	BattleRendering();
};


#endif
