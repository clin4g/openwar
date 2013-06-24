// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleRendering.h"


BattleRendering::BattleRendering()
{
	_ground_texture_renderer = new renderer<texture_vertex3, ground_texture_uniforms>((
			VERTEX_ATTRIBUTE(texture_vertex3, _position),
					VERTEX_ATTRIBUTE(texture_vertex3, _texcoord),
					SHADER_UNIFORM(ground_texture_uniforms, _transform),
					SHADER_UNIFORM(ground_texture_uniforms, _texture),
					VERTEX_SHADER
		({
						uniform
						mat4 transform;
						attribute
						vec3 position;
						attribute
						vec2 texcoord;
						varying
						vec2 _texcoord;

						void main()
						{
							vec4 p = transform * vec4(position, 1);

							_texcoord = texcoord;

							gl_Position = p;
							gl_PointSize = 1.0;
						}
					}),
					FRAGMENT_SHADER
		({
						uniform
						sampler2D texture;
						varying
						vec2 _texcoord;

						void main()
						{
							gl_FragColor = texture2D(texture, _texcoord);
						}
					})
	));
	_ground_texture_renderer->_blend_sfactor = GL_ONE;
	_ground_texture_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;


	_ground_shadow_renderer = new renderer<plain_vertex, plain_uniforms>((
			VERTEX_ATTRIBUTE(plain_vertex, _position),
					SHADER_UNIFORM(plain_uniforms, _transform),
					VERTEX_SHADER
		({
						attribute
						vec2 position;
						uniform
						mat4 transform;
						varying
						vec2 _groundpos;

						void main()
						{
							vec4 p = transform * vec4(position, -2.5, 1);

							_groundpos = position;

							gl_Position = p;
							gl_PointSize = 1.0;
						}
					}),
					FRAGMENT_SHADER
		({
						varying
						vec2 _groundpos;

						void main()
						{
							float d = distance(_groundpos, vec2(512.0, 512.0)) - 512.0;
							float a = clamp(0.3 - d / 20.0, 0.0, 0.3);

							gl_FragColor = vec4(0, 0, 0, a);
						}
					}))
	);
	_ground_shadow_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_ground_shadow_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_textureUnitMarkers = new texture(@"Texture256x256.png");
	_textureMovementBlue = new texture(@"MovementBlue16x16.png");
	_textureMovementGray = new texture(@"MovementGray16x16.png");
	_textureMovementRed = new texture(@"MovementRed16x16.png");
	_textureMissileBlue = new texture(@"MissileBlue16x16.png");
	_textureMissileGray = new texture(@"MissileGray16x16.png");
	_textureMissileRed = new texture(@"MissileRed16x16.png");
	_textureTouchMarker = new texture(@"TouchMarker.png");
}
