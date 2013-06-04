/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "BattleRendering.h"


renderer<BattleRendering::texture_billboard_vertex, BattleRendering::texture_billboard_uniforms>* BattleRendering::_texture_billboard_renderer = nullptr;
renderer<BattleRendering::color_billboard_vertex, BattleRendering::color_billboard_uniforms>* BattleRendering::_color_billboard_renderer = nullptr;
renderer<color_vertex3, BattleRendering::ground_gradient_uniforms>* BattleRendering::_ground_gradient_renderer = nullptr;
renderer<plain_vertex3, BattleRendering::ground_color_uniforms>* BattleRendering::_ground_plain_renderer = nullptr;
renderer<texture_vertex3, BattleRendering::ground_texture_uniforms>* BattleRendering::_ground_texture_renderer = nullptr;
renderer<plain_vertex, plain_uniforms>* BattleRendering::_ground_shadow_renderer = nullptr;

renderer<plain_vertex, BattleRendering::ground_texture_uniforms>* BattleRendering::_water_inside_renderer = nullptr;
renderer<plain_vertex, BattleRendering::ground_texture_uniforms>* BattleRendering::_water_border_renderer = nullptr;


texture* BattleRendering::_textureBackgroundLinen = nullptr;
texture* BattleRendering::_textureBackgroundTerrain = nullptr;
texture* BattleRendering::_textureUnitMarkers = nullptr;
texture* BattleRendering::_textureMovementBlue = nullptr;
texture* BattleRendering::_textureMovementGray = nullptr;
texture* BattleRendering::_textureMovementRed = nullptr;
texture* BattleRendering::_textureMissileBlue = nullptr;
texture* BattleRendering::_textureMissileGray = nullptr;
texture* BattleRendering::_textureMissileRed = nullptr;
texture* BattleRendering::_textureBillboards = nullptr;
texture* BattleRendering::_textureTouchMarker = nullptr;


void BattleRendering::Initialize()
{
	_texture_billboard_renderer = new renderer<texture_billboard_vertex, texture_billboard_uniforms>((
			VERTEX_ATTRIBUTE(texture_billboard_vertex, _position),
					VERTEX_ATTRIBUTE(texture_billboard_vertex, _height),
					VERTEX_ATTRIBUTE(texture_billboard_vertex, _texcoord),
					VERTEX_ATTRIBUTE(texture_billboard_vertex, _texsize),
					SHADER_UNIFORM(texture_billboard_uniforms, _transform),
					SHADER_UNIFORM(texture_billboard_uniforms, _texture),
					SHADER_UNIFORM(texture_billboard_uniforms, _upvector),
					SHADER_UNIFORM(texture_billboard_uniforms, _viewport_height),
					SHADER_UNIFORM(texture_billboard_uniforms, _min_point_size),
					SHADER_UNIFORM(texture_billboard_uniforms, _max_point_size),
					VERTEX_SHADER
	    ({
						uniform
						mat4 transform;
						uniform
						vec3 upvector;
						uniform float viewport_height;
						uniform float min_point_size;
						uniform float max_point_size;
						attribute
						vec3 position;
						attribute float height;
						attribute
						vec2 texcoord;
						attribute
						vec2 texsize;
						varying
						vec2 _texcoord;
						varying
						vec2 _texsize;

						void main()
						{
							float scale = 0.5 * height * viewport_height;
							vec3 position2 = position + scale * upvector;
							vec4 p = transform * vec4(position, 1);
							vec4 q = transform * vec4(position2, 1);
							float s = clamp(abs(q.y / q.w - p.y / p.w), min_point_size, max_point_size);

							_texcoord = texcoord;
							_texsize = texsize;

							gl_Position = p;
							gl_PointSize = s;
						}
					}),
					FRAGMENT_SHADER
		({
						uniform
						sampler2D texture;
						varying
						vec2 _texcoord;
						varying
						vec2 _texsize;

						void main()
						{
							vec4 color = texture2D(texture, _texcoord + gl_PointCoord * _texsize);

							gl_FragColor = color;
						}
					})));
	_texture_billboard_renderer->_blend_sfactor = GL_ONE;
	_texture_billboard_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;


	_color_billboard_renderer = new renderer<color_billboard_vertex, color_billboard_uniforms>((
			VERTEX_ATTRIBUTE(color_billboard_vertex, _position),
					VERTEX_ATTRIBUTE(color_billboard_vertex, _color),
					VERTEX_ATTRIBUTE(color_billboard_vertex, _height),
					SHADER_UNIFORM(color_billboard_uniforms, _transform),
					SHADER_UNIFORM(color_billboard_uniforms, _upvector),
					SHADER_UNIFORM(color_billboard_uniforms, _viewport_height),
					VERTEX_SHADER
		({
						uniform
						mat4 transform;
						uniform
						vec3 upvector;
						uniform float viewport_height;
						attribute
						vec3 position;
						attribute
						vec4 color;
						attribute float height;
						varying
						vec4 _color;

						void main()
						{
							float scale = 0.5 * height * viewport_height;
							vec3 position2 = position + scale * upvector;
							vec4 p = transform * vec4(position, 1);
							vec4 q = transform * vec4(position2, 1);
							float s = abs(q.y / q.w - p.y / p.w);

							float a = color.a;
							if (s < 1.0)
							{
								a = a * s;
								s = 1.0;
							}

							_color = vec4(color.rgb, a);
							gl_Position = p;
							gl_PointSize = s;
						}
					}),
					FRAGMENT_SHADER
		({
						varying
						vec4 _color;

						void main()
						{
							gl_FragColor = _color;
						}
					})));
	_color_billboard_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_color_billboard_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;


	_ground_gradient_renderer = new renderer<color_vertex3, ground_gradient_uniforms>((
			VERTEX_ATTRIBUTE(color_vertex3, _position),
					VERTEX_ATTRIBUTE(color_vertex3, _color),
					SHADER_UNIFORM(ground_gradient_uniforms, _transform),
					SHADER_UNIFORM(ground_gradient_uniforms, _point_size),
					VERTEX_SHADER
		({
						attribute
						vec3 position;
						attribute
						vec4 color;
						uniform
						mat4 transform;
						uniform float point_size;
						varying
						vec4 v_color;

						void main()
						{
							vec4 p = transform * vec4(position, 1);

							v_color = color;

							gl_Position = p;
							gl_PointSize = point_size;
						}
					}),
					FRAGMENT_SHADER
		({
						varying
						vec4 v_color;

						void main()
						{
							gl_FragColor = v_color;
						}
					})
	));
	_ground_gradient_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_ground_gradient_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;


	_ground_plain_renderer = new renderer<plain_vertex3, ground_color_uniforms>((
			VERTEX_ATTRIBUTE(plain_vertex3, _position),
					SHADER_UNIFORM(ground_color_uniforms, _transform),
					SHADER_UNIFORM(ground_color_uniforms, _point_size),
					SHADER_UNIFORM(ground_color_uniforms, _color),
					VERTEX_SHADER
		({
						attribute
						vec3 position;
						uniform
						mat4 transform;
						uniform float point_size;

						void main()
						{
							vec4 p = transform * vec4(position, 1);

							gl_Position = p;
							gl_PointSize = point_size;
						}
					}),
					FRAGMENT_SHADER
		({
						uniform
						vec4 color;

						void main()
						{
							gl_FragColor = color;
						}
					}))
	);
	_ground_plain_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_ground_plain_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



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

							_groundpos = position.xy;

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
							float a = clamp(0.0, 1.0, 0.3 - d / 20.0);

							gl_FragColor = vec4(0, 0, 0, a);
						}
					}))
	);
	_ground_shadow_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_ground_shadow_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_water_inside_renderer = new renderer<plain_vertex, ground_texture_uniforms>((
			VERTEX_ATTRIBUTE(plain_vertex, _position),
					SHADER_UNIFORM(ground_texture_uniforms, _transform),
					SHADER_UNIFORM(ground_texture_uniforms, _texture),
					VERTEX_SHADER
		({
						attribute
						vec2 position;
						uniform
						mat4 transform;

						void main()
						{
							vec4 p = transform * vec4(position, 0, 1);

							gl_Position = p;
							gl_PointSize = 1.0;
						}
					}),
					FRAGMENT_SHADER
		({
						void main()
						{
							gl_FragColor = vec4(0.44 * 0.5, 0.72 * 0.5, 0.91 * 0.5, 0.5);
						}
					})
	));
	_water_inside_renderer->_blend_sfactor = GL_ONE;
	_water_inside_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_water_border_renderer = new renderer<plain_vertex, ground_texture_uniforms>((
			VERTEX_ATTRIBUTE(plain_vertex, _position),
					SHADER_UNIFORM(ground_texture_uniforms, _transform),
					SHADER_UNIFORM(ground_texture_uniforms, _texture),
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
							vec4 p = transform * vec4(position, 0, 1);

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
							if (distance(_groundpos, vec2(512.0, 512.0)) > 512.0)
								discard;

							gl_FragColor = vec4(0.44 * 0.5, 0.72 * 0.5, 0.91 * 0.5, 0.5);
						}
					})
	));
	_water_border_renderer->_blend_sfactor = GL_ONE;
	_water_border_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_textureBackgroundLinen = new texture(@"Linen128x128.png");
	_textureBackgroundTerrain = new texture(@"Ground64x64.png");
	_textureUnitMarkers = new texture(@"Texture256x256.png");
	_textureMovementBlue = new texture(@"MovementBlue16x16.png");
	_textureMovementGray = new texture(@"MovementGray16x16.png");
	_textureMovementRed = new texture(@"MovementRed16x16.png");
	_textureMissileBlue = new texture(@"MissileBlue16x16.png");
	_textureMissileGray = new texture(@"MissileGray16x16.png");
	_textureMissileRed = new texture(@"MissileRed16x16.png");
	_textureBillboards = new texture(@"Billboards.png");
	_textureTouchMarker = new texture(@"TouchMarker.png");
}
