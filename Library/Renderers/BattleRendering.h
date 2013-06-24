// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLERENDERING_H
#define BATTLERENDERING_H

#include "renderer.h"
#include "TextureBillboardRenderer.h"


struct BattleRendering
{
	struct ground_texture_uniforms
	{
		glm::mat4x4 _transform;
		const texture* _texture;
	};

	vertexbuffer<plain_vertex> _vboTerrainShadow;
	vertexbuffer<texture_vertex3> _vboMovementMarkerPath;
	vertexbuffer<texture_vertex3> _vboTrackingMarkerPath;
	vertexbuffer<texture_vertex3> _vboTrackingMarkerOrientation;
	vertexbuffer<texture_vertex3> _vboTrackingMarkerMissileHead;
	vertexbuffer<texture_vertex3> _vboUnitMarkerTargetLine;
	vertexbuffer<texture_vertex3> _vboUnitMarkerTargetHead;
	vertexbuffer<texture_vertex> _vboTrackingMarkerShadow;

	vertexbuffer<texture_billboard_vertex> _vboTextureBillboards1;

	renderer<texture_vertex3, ground_texture_uniforms>* _ground_texture_renderer;
	renderer<plain_vertex, plain_uniforms>* _ground_shadow_renderer;

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
