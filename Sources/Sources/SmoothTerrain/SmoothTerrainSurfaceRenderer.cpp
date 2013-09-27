// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmoothTerrainSurfaceRenderer.h"
#include "../../Library/Algebra/image.h"



terrain_renderers::terrain_renderers() :
_terrain_inside(nullptr),
_terrain_border(nullptr),
_terrain_skirt(nullptr),
_depth_inside(nullptr),
_depth_border(nullptr),
_depth_skirt(nullptr),
_sobel_filter(nullptr),
_ground_shadow(nullptr)
{
}


terrain_renderers::~terrain_renderers()
{
	delete _terrain_inside;
	delete _terrain_border;
	delete _terrain_skirt;
	delete _depth_inside;
	delete _depth_border;
	delete _depth_skirt;
	delete _sobel_filter;
	delete _ground_shadow;
}


void terrain_renderers::render_terrain_inside(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms)
{
	if (_terrain_inside == nullptr)
	{
		_terrain_inside = new renderer<terrain_vertex, terrain_uniforms>((
			VERTEX_ATTRIBUTE(terrain_vertex, _position),
			VERTEX_ATTRIBUTE(terrain_vertex, _normal),
			SHADER_UNIFORM(terrain_uniforms, _transform),
			SHADER_UNIFORM(terrain_uniforms, _map_bounds),
			SHADER_UNIFORM(terrain_uniforms, _light_normal),
			SHADER_UNIFORM(terrain_uniforms, _colormap),
			SHADER_UNIFORM(terrain_uniforms, _splatmap),
			VERTEX_SHADER
			({
				uniform mat4 transform;
				uniform vec4 map_bounds;
				uniform vec3 light_normal;

				attribute vec3 position;
				attribute vec3 normal;

				varying vec3 _position;
				varying vec2 _colorcoord;
				varying vec2 _splatcoord;
				varying float _brightness;

				void main()
				{
					vec4 p = transform * vec4(position, 1);

					float brightness = -dot(light_normal, normal);

					_position = position;
					_colorcoord = vec2(brightness, 1.0 - (2.5 + position.z) / 128.0);
					_splatcoord = (position.xy - map_bounds.xy) / map_bounds.zw;
					_brightness = brightness;


				    gl_Position = p;
					gl_PointSize = 1.0;
				}
			}),
			FRAGMENT_SHADER
			({
				uniform sampler2D colormap;
				uniform sampler2D splatmap;

				varying vec3 _position;
				varying vec2 _colorcoord;
				varying vec2 _splatcoord;
				varying float _brightness;

				void main()
				{
					vec3 color = texture2D(colormap, _colorcoord).rgb;
					vec3 splat = texture2D(splatmap, _splatcoord).rgb;

					float f = step(0.0, _position.z) * smoothstep(0.475, 0.525, splat.g);
					color = mix(color, vec3(0.2196, 0.3608, 0.1922), 0.25 * f);
					color = mix(color, vec3(0), 0.03 * step(0.5, 1.0 - _brightness));
					color = mix(color, vec3(0.4), 0.3333 * step(0.5, splat.r));

				    gl_FragColor = vec4(color, 1.0);
				}
			})
		));
		_terrain_inside->_blend_sfactor = GL_ONE;
		_terrain_inside->_blend_dfactor = GL_ZERO;
	}
	_terrain_inside->render(shape, uniforms);
}



void terrain_renderers::render_terrain_border(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms)
{
	if (_terrain_border == nullptr)
	{
		_terrain_border = new renderer<terrain_vertex, terrain_uniforms>((
			VERTEX_ATTRIBUTE(terrain_vertex, _position),
			VERTEX_ATTRIBUTE(terrain_vertex, _normal),
			SHADER_UNIFORM(terrain_uniforms, _transform),
			SHADER_UNIFORM(terrain_uniforms, _light_normal),
			SHADER_UNIFORM(terrain_uniforms, _map_bounds),
			SHADER_UNIFORM(terrain_uniforms, _colormap),
			SHADER_UNIFORM(terrain_uniforms, _splatmap),
			VERTEX_SHADER
			({
				uniform mat4 transform;
				uniform vec4 map_bounds;
				uniform vec3 light_normal;

				attribute vec3 position;
				attribute vec3 normal;

				varying vec3 _position;
				varying vec2 _colorcoord;
				varying vec2 _splatcoord;
				varying float _brightness;

				void main()
				{
					vec4 p = transform * vec4(position, 1);

					float brightness = -dot(light_normal, normal);

					_position = position;
					_colorcoord = vec2(brightness, 1.0 - (2.5 + position.z) / 128.0);
					_splatcoord = (position.xy - map_bounds.xy) / map_bounds.zw;
					_brightness = brightness;

				    gl_Position = p;
					gl_PointSize = 1.0;
				}
			}),
			FRAGMENT_SHADER
			({
				uniform sampler2D colormap;
				uniform sampler2D splatmap;

				varying vec3 _position;
				varying vec2 _colorcoord;
				varying vec2 _splatcoord;
				varying float _brightness;

				void main()
				{
					if (distance(_splatcoord, vec2(0.5, 0.5)) > 0.5)
						discard;

					vec3 color = texture2D(colormap, _colorcoord).rgb;
					vec3 splat = texture2D(splatmap, _splatcoord).rgb;

					float f = step(0.0, _position.z) * smoothstep(0.475, 0.525, splat.g);
					color = mix(color, vec3(0.2196, 0.3608, 0.1922), 0.3 * f);
					color = mix(color, vec3(0), 0.03 * step(0.5, 1.0 - _brightness));
					color = mix(color, vec3(0.4), 0.3333 * step(0.5, splat.r));

				    gl_FragColor = vec4(color, 1.0);
				}
			})
		));
		_terrain_border->_blend_sfactor = GL_ONE;
		_terrain_border->_blend_dfactor = GL_ZERO;
	}
	_terrain_border->render(shape, uniforms);
}



void terrain_renderers::render_terrain_skirt(vertexbuffer<skirt_vertex>& shape, const texture_uniforms& uniforms)
{
	if (_terrain_skirt == nullptr)
	{
		_terrain_skirt = new renderer<skirt_vertex, texture_uniforms>((
			VERTEX_ATTRIBUTE(skirt_vertex, _position),
			VERTEX_ATTRIBUTE(skirt_vertex, _height),
			SHADER_UNIFORM(texture_uniforms, _transform),
			SHADER_UNIFORM(texture_uniforms, _texture),
			VERTEX_SHADER
			({
				attribute vec3 position;
				attribute float height;
				uniform mat4 transform;
				varying vec2 _colorcoord;
				varying float _height;

				void main()
				{
					vec4 p = transform * vec4(position, 1);

					_colorcoord = vec2(0.2, 1.0 - (2.5 + height) / 128.0);
					_height = 0.85 * (position.z + 2.5) / (height + 2.5);

				    gl_Position = p;
				}
			}),
			FRAGMENT_SHADER
			({
				uniform sampler2D texture;
				varying vec2 _colorcoord;
				varying float _height;

				void main()
				{
					vec3 color = texture2D(texture, _colorcoord).rgb;
					color = mix(vec3(0.15), color, _height);

				    gl_FragColor = vec4(color, 1);
				}
			}))
		);
		_terrain_skirt->_blend_sfactor = GL_ONE;
		_terrain_skirt->_blend_dfactor = GL_ZERO;
	}
	_terrain_skirt->render(shape, uniforms);
}



void terrain_renderers::render_depth_inside(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms)
{
	if (_depth_inside == nullptr)
	{
		_depth_inside = new renderer<terrain_vertex, terrain_uniforms>((
			VERTEX_ATTRIBUTE(terrain_vertex, _position),
			VERTEX_ATTRIBUTE(terrain_vertex, _normal),
			SHADER_UNIFORM(terrain_uniforms, _transform),
			VERTEX_SHADER
			({
				uniform mat4 transform;
				attribute vec3 position;
				attribute vec3 normal;

				void main()
				{
					vec4 p = transform * vec4(position, 1);
				    gl_Position = p;
				}
			}),
			FRAGMENT_SHADER
			({
				void main()
				{
				    gl_FragColor = vec4(1, 1, 1, 1);
				}
			})
		));
		_depth_inside->_blend_sfactor = GL_ONE;
		_depth_inside->_blend_dfactor = GL_ZERO;
	}
	_depth_inside->render(shape, uniforms);
}



void terrain_renderers::render_depth_border(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms)
{
	if (_depth_border == nullptr)
	{
		_depth_border = new renderer<terrain_vertex, terrain_uniforms>((
			VERTEX_ATTRIBUTE(terrain_vertex, _position),
			VERTEX_ATTRIBUTE(terrain_vertex, _normal),
			SHADER_UNIFORM(terrain_uniforms, _transform),
			SHADER_UNIFORM(terrain_uniforms, _map_bounds),
			VERTEX_SHADER
			({
				uniform mat4 transform;
				uniform vec4 map_bounds;
				attribute vec3 position;
				attribute vec3 normal;
				varying vec2 _terraincoord;

				void main()
				{
					_terraincoord = (position.xy - map_bounds.xy) / map_bounds.zw;
					vec4 p = transform * vec4(position, 1);
				    gl_Position = p;
				}
			}),
			FRAGMENT_SHADER
			({
				varying vec2 _terraincoord;

				void main()
				{
					if (distance(_terraincoord, vec2(0.5, 0.5)) > 0.5)
						discard;

				    gl_FragColor = vec4(1);
				}
			})
		));
		_depth_border->_blend_sfactor = GL_ONE;
		_depth_border->_blend_dfactor = GL_ZERO;
	}
	_depth_border->render(shape, uniforms);
}



void terrain_renderers::render_depth_skirt(vertexbuffer<skirt_vertex>& shape, const plain_uniforms& uniforms)
{
	if (_depth_skirt == nullptr)
	{
		_depth_skirt = new renderer<skirt_vertex, plain_uniforms>((
			VERTEX_ATTRIBUTE(skirt_vertex, _position),
			VERTEX_ATTRIBUTE(skirt_vertex, _height),
			SHADER_UNIFORM(plain_uniforms, _transform),
			VERTEX_SHADER
			({
				uniform mat4 transform;
				attribute vec3 position;
				attribute float height;

				void main()
				{
					vec4 p = transform * vec4(position, 1);

				    gl_Position = p;
				}
			}),
			FRAGMENT_SHADER
			({
				void main()
				{
				    gl_FragColor = vec4(1);
				}
			}))
		);
		_depth_skirt->_blend_sfactor = GL_ONE;
		_depth_skirt->_blend_dfactor = GL_ZERO;
	}
	_depth_skirt->render(shape, uniforms);
}


void terrain_renderers::render_sobel_filter(vertexbuffer<texture_vertex>& shape, const sobel_uniforms& uniforms)
{
	if (_sobel_filter == nullptr)
	{
		_sobel_filter = new renderer<texture_vertex, sobel_uniforms>((
			VERTEX_ATTRIBUTE(texture_vertex, _position),
			VERTEX_ATTRIBUTE(texture_vertex, _texcoord),
			SHADER_UNIFORM(sobel_uniforms, _transform),
			SHADER_UNIFORM(sobel_uniforms, _depth),
			VERTEX_SHADER
			({
				uniform mat4 transform;
				attribute vec2 position;
				attribute vec2 texcoord;

				varying vec2 coord11;
				//varying vec2 coord12;
				varying vec2 coord13;
				//varying vec2 coord21;
				//varying vec2 coord23;
				varying vec2 coord31;
				//varying vec2 coord32;
				varying vec2 coord33;

				void main()
				{
					const float dx = 1.0 / 2.0 / 1024.0;
					const float dy = 1.0 / 2.0 / 768.0;

					vec4 p = transform * vec4(position, 0, 1);

				    gl_Position = p;

					coord11 = texcoord + vec2(-dx,  dy);
					//coord12 = texcoord + vec2(0.0,  dy);
					coord13 = texcoord + vec2( dx,  dy);
					//coord21 = texcoord + vec2(-dx, 0.0);
					//coord23 = texcoord + vec2( dx, 0.0);
					coord31 = texcoord + vec2(-dx, -dy);
					//coord32 = texcoord + vec2(0.0, -dy);
					coord33 = texcoord + vec2( dx, -dy);
				}
			}),
			FRAGMENT_SHADER
			({
				uniform sampler2D depth;

				varying vec2 coord11;
				//varying vec2 coord12;
				varying vec2 coord13;
				//varying vec2 coord21;
				//varying vec2 coord23;
				varying vec2 coord31;
				//varying vec2 coord32;
				varying vec2 coord33;

				void main()
				{
					float value11 = texture2D(depth, coord11).r;
					//float value12 = texture2D(depth, coord12).r;
					float value13 = texture2D(depth, coord13).r;
					//float value21 = texture2D(depth, coord21).r;
					//float value23 = texture2D(depth, coord23).r;
					float value31 = texture2D(depth, coord31).r;
					//float value32 = texture2D(depth, coord32).r;
					float value33 = texture2D(depth, coord33).r;

					float h = value11 - value33;
					float v = value31 - value13;

					//float h = -value11 - 2.0 * value12 - value13 + value31 + 2.0 * value32 + value33;
					//float v = -value31 - 2.0 * value21 - value11 + value33 + 2.0 * value23 + value13;

					float k = clamp(5.0 * length(vec2(h, v)), 0.0, 0.6);

					//gl_FragColor = vec4(0.145, 0.302, 0.255, k);
					gl_FragColor = vec4(0.0725, 0.151, 0.1275, k);
				}
			})
		));
		_sobel_filter->_blend_sfactor = GL_SRC_ALPHA;
		_sobel_filter->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
	}
	_sobel_filter->render(shape, uniforms);
}


void terrain_renderers::render_ground_shadow(vertexbuffer<plain_vertex>& shape, const terrain_uniforms& uniforms)
{
	if (_ground_shadow == nullptr)
	{
		_ground_shadow = new renderer<plain_vertex, terrain_uniforms>((
			VERTEX_ATTRIBUTE(plain_vertex, _position),
				SHADER_UNIFORM(terrain_uniforms, _transform),
				SHADER_UNIFORM(terrain_uniforms, _map_bounds),
				VERTEX_SHADER
			({
					uniform mat4 transform;
					uniform vec4 map_bounds;
					attribute vec2 position;
					varying vec2 _groundpos;

					void main()
					{
						vec4 p = transform * vec4(position, -2.5, 1);

						_groundpos = (position - map_bounds.xy) / map_bounds.zw;

						gl_Position = p;
						gl_PointSize = 1.0;
					}
				}),
				FRAGMENT_SHADER
			({
					varying vec2 _groundpos;

					void main()
					{
						float d = distance(_groundpos, vec2(0.5, 0.5)) - 0.5;
						float a = clamp(0.3 - d * 24.0, 0.0, 0.3);

						gl_FragColor = vec4(0, 0, 0, a);
					}
				}))
		);
		_ground_shadow->_blend_sfactor = GL_SRC_ALPHA;
		_ground_shadow->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
	}

	_ground_shadow->render(shape, uniforms);
}




static int _colorScheme = 0;


static glm::vec3 heightcolor(float h)
{
	static std::vector<std::pair<float, glm::vec3>>* _colors = nullptr;
	if (_colors == nullptr)
	{
		_colors = new std::vector<std::pair<float, glm::vec3>>();
		switch (_colorScheme)
		{
			case 1:
				_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(164, 146, 124) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(-0.5, glm::vec3(219, 186, 153) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(0.0,  glm::vec3(191, 171, 129) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(6.5,  glm::vec3(114, 150, 65) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(7.0,  glm::vec3(120, 150, 64) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(10,   glm::vec3(135, 149, 60) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(50,   glm::vec3(132, 137, 11) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(132, 137, 11) / 255.0f));
				break;

			case 2: // Granicus
				_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(156, 137, 116) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(0.5, glm::vec3(156, 137, 116) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(1.0,  glm::vec3(128, 137, 74) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(50,  glm::vec3(72, 67, 38) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(72, 67, 38) / 255.0f));
				break;

			case 3: // Issus
				_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(204, 168, 146) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(0.5, glm::vec3(204, 168, 146) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(1.0,  glm::vec3(221, 138, 88) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(50,  glm::vec3(197, 111, 60) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(197, 111, 60) / 255.0f));
				break;

			case 4: // Hydaspes
				_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(138, 153, 105) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(0.5, glm::vec3(144, 149, 110) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(1.0,  glm::vec3(128, 137, 74) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(50,  glm::vec3(72, 67, 38) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(72, 67, 38) / 255.0f));
				break;

			default: // samurai
				_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(164, 146, 124) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(-0.5, glm::vec3(219, 186, 153) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(0.0,  glm::vec3(194, 142, 102) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(6.5,  glm::vec3(199, 172, 148) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(7.0,  glm::vec3(177, 172, 132) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(10,   glm::vec3(125, 171, 142) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(50,   glm::vec3(59,  137,  11) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(59,  137,  11) / 255.0f));
				break;
		}
	}

	int i = 0;
	while (i + 1 < (int)_colors->size() && h > (*_colors)[i + 1].first)
		++i;

	float h1 = (*_colors)[i].first;
	float h2 = (*_colors)[i + 1].first;
	glm::vec3 c1 = (*_colors)[i].second;
	glm::vec3 c2 = (*_colors)[i + 1].second;

	return glm::mix(c1, c2, (bounds1f(h1, h2).clamp(h) - h1) / (h2 - h1));
}


static glm::vec3 adjust_brightness(glm::vec3 c, float brightness)
{
	if (brightness < 0.5f)
		return c * (1.0f - 0.2f * (0.5f - brightness));
	else if (brightness > 0.83f)
		return glm::mix(c, glm::vec3(1.0f), 0.3f * (brightness - 0.5f));
	else
		return glm::mix(c, glm::vec3(1.0f), 0.2f * (brightness - 0.5f));
}


texture* terrain_renderers::create_colormap()
{
	static glm::vec3 r[256];

	for (int i = 0; i < 256; ++i)
	{
		r[i].r = (rand() & 0x7fff) / (float)0x7fff;
		r[i].g = (rand() & 0x7fff) / (float)0x7fff;
		r[i].b = (rand() & 0x7fff) / (float)0x7fff;
	}

	image img(64, 256);
	for (int x = 0; x < 64; ++x)
		for (int y = 0; y < 256; ++y)
		{
			float brightness = x / 63.0f;
			float h = -2.5f + 0.5f * y;
			glm::vec3 c = heightcolor(h);
			c = adjust_brightness(c, brightness);
			if (h > 0)
				c = glm::mix(c, r[y], 0.015f);

			img.set_pixel(x, 255 - y, glm::vec4(c, 1));
		}

	//NSData* data = ConvertImageToTiff(&img);
	//[data writeToFile:@"/Users/nicke/Desktop/height.tiff" atomically:YES];

	texture* result = new texture(img);

	glBindTexture(GL_TEXTURE_2D, result->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return result;
}
