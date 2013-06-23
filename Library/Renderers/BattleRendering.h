// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLERENDERING_H
#define BATTLERENDERING_H

#include "renderer.h"
#include "TextureBillboardRenderer.h"


struct BattleRendering
{
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

	vertexbuffer<plain_vertex> _vboTerrainShadow;
	vertexbuffer<color_vertex3> _vboRangeMarker;
	vertexbuffer<texture_vertex3> _vboMovementMarkerPath;
	vertexbuffer<texture_vertex3> _vboTrackingMarkerPath;
	vertexbuffer<texture_vertex3> _vboTrackingMarkerOrientation;
	vertexbuffer<texture_vertex3> _vboTrackingMarkerMissileHead;
	vertexbuffer<texture_vertex3> _vboUnitMarkerTargetLine;
	vertexbuffer<texture_vertex3> _vboUnitMarkerTargetHead;
	vertexbuffer<texture_vertex> _vboTrackingMarkerShadow;

	vertexbuffer<texture_billboard_vertex> _vboTextureBillboards1;

	renderer<color_vertex3, ground_gradient_uniforms>* _ground_gradient_renderer;
	renderer<texture_vertex3, ground_texture_uniforms>* _ground_texture_renderer;
	renderer<plain_vertex, plain_uniforms>* _ground_shadow_renderer;

	texture* _textureBackgroundLinen;
	texture* _textureUnitMarkers;
	texture* _textureMovementBlue;
	texture* _textureMovementGray;
	texture* _textureMovementRed;
	texture* _textureMissileBlue;
	texture* _textureMissileGray;
	texture* _textureMissileRed;
	texture* _textureTouchMarker;

	BattleRendering();
};


#endif
