// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmoothTerrainSurfaceRenderer.h"
#include "../../Library/Algebra/image.h"



struct terrain_renderers
{
	renderer<terrain_vertex, terrain_uniforms>* _terrain_inside;
	renderer<terrain_vertex, terrain_uniforms>* _terrain_border;
	renderer<skirt_vertex, texture_uniforms>* _terrain_skirt;
	renderer<terrain_vertex, terrain_uniforms>* _depth_inside;
	renderer<terrain_vertex, terrain_uniforms>* _depth_border;
	renderer<skirt_vertex, plain_uniforms>* _depth_skirt;
	renderer<texture_vertex, sobel_uniforms>* _sobel_filter;
	renderer<plain_vertex, terrain_uniforms>* _ground_shadow;

	terrain_renderers();
	~terrain_renderers();

	void render_terrain_inside(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_terrain_border(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_terrain_skirt(vertexbuffer<skirt_vertex>& shape, const texture_uniforms& uniforms);

	void render_depth_inside(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_depth_border(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_depth_skirt(vertexbuffer<skirt_vertex>& shape, const plain_uniforms& uniforms);

	void render_sobel_filter(vertexbuffer<texture_vertex>& shape, const sobel_uniforms& uniforms);

	void render_ground_shadow(vertexbuffer<plain_vertex>& shape, const terrain_uniforms& uniforms);
};



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
					_height = (position.z + 2.5) / (height + 2.5);

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
	else
		return glm::mix(c, glm::vec3(1.0f), 0.2f * (brightness - 0.5f));
}


static texture* create_colormap()
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

	return new texture(img);
}


#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES // detect objective-c
static NSString* FramebufferStatusString(GLenum status)
{
	switch (status)
	{
		case GL_FRAMEBUFFER_COMPLETE: return @"GL_FRAMEBUFFER_COMPLETE";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return @"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return @"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: return @"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
#endif
		case GL_FRAMEBUFFER_UNSUPPORTED: return @"GL_FRAMEBUFFER_UNSUPPORTED";
		default: return [NSString stringWithFormat:@"0x%04x", (unsigned int)status];
	}
}
#endif



SmoothTerrainSurfaceRenderer::SmoothTerrainSurfaceRenderer(SmoothTerrainSurface* terrainSurfaceModel, bool render_edges) :
_terrainSurfaceModel(terrainSurfaceModel),
_framebuffer_width(0),
_framebuffer_height(0),
_framebuffer(nullptr),
_colorbuffer(nullptr),
_depth(nullptr),
_colormap(nullptr),
_splatmap(nullptr),
_splatmapImage(nullptr),
_size(255),
_heights(nullptr),
_normals(nullptr)
{
	terrainSurfaceModel->_renderer = this;

	_renderers = new terrain_renderers();

	if (render_edges)
	{
		_depth = new texture();
		glBindTexture(GL_TEXTURE_2D, _depth->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		UpdateDepthTextureSize();

		_framebuffer = new framebuffer();

#if !TARGET_OS_IPHONE
		_colorbuffer = new renderbuffer(GL_RGBA, _framebuffer_width, _framebuffer_height);
		_framebuffer->attach_color(_colorbuffer);
#endif

		_framebuffer->attach_depth(_depth);
		{
			bind_framebuffer binding(*_framebuffer);
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE)
			{
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
				NSLog(@"CheckGLFramebuffer %@", FramebufferStatusString(status));
#endif
			}
		}
	}

	_colormap = create_colormap();
	glBindTexture(GL_TEXTURE_2D, _colormap->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	_heights = new float[_size * _size];
	_normals = new glm::vec3[_size * _size];
	UpdateHeights();
	UpdateNormals();


	_splatmap = new texture();
	UpdateSplatmap();

	InitializeSkirt();
	InitializeShadow();
	InitializeLines();

	BuildTriangles();
}



SmoothTerrainSurfaceRenderer::~SmoothTerrainSurfaceRenderer()
{
	delete _colormap;
	delete _splatmap;
	delete _splatmapImage;
	delete _framebuffer;
	delete _colorbuffer;
	delete _depth;
	delete _renderers;
}



void SmoothTerrainSurfaceRenderer::Render(const glm::mat4x4& transform, const glm::vec3& lightNormal)
{
	glm::vec4 map_bounds = glm::vec4(GetSourceBounds().min, GetSourceBounds().size());

	glDepthMask(false);

	terrain_uniforms shadow_uniforms;
	shadow_uniforms._transform = transform;
	shadow_uniforms._map_bounds = map_bounds;

	_renderers->render_ground_shadow(_vboShadow, shadow_uniforms);

	glDepthMask(true);

	terrain_uniforms uniforms;
	uniforms._transform = transform;
	uniforms._map_bounds = map_bounds;
	uniforms._light_normal = lightNormal;
	uniforms._colormap = _colormap;
	uniforms._splatmap = _splatmap;


	if (_framebuffer != nullptr)
	{
		UpdateDepthTextureSize();

		bind_framebuffer binding(*_framebuffer);

		glClear(GL_DEPTH_BUFFER_BIT);

		terrain_uniforms du;
		du._transform = uniforms._transform;
		du._map_bounds = map_bounds;

		_renderers->render_depth_inside(_vboInside, du);
		_renderers->render_depth_border(_vboBorder, du);

		plain_uniforms pu;
		pu._transform = uniforms._transform;
		_renderers->render_depth_skirt(_vboSkirt, pu);
	}

	_renderers->render_terrain_inside(_vboInside, uniforms);
	_renderers->render_terrain_border(_vboBorder, uniforms);

	bool showLines = this == nullptr;
	if (showLines)
	{
		glDisable(GL_DEPTH_TEST);
		gradient_uniforms g;
		g._transform = uniforms._transform;
		renderers::singleton->_gradient_renderer3->render(_vboLines, g);
		glEnable(GL_DEPTH_TEST);
	}

	texture_uniforms tu;
	tu._transform = uniforms._transform;
	tu._texture = _colormap;
	_renderers->render_terrain_skirt(_vboSkirt, tu);

	if (_depth != nullptr)
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);

		vertexbuffer<texture_vertex> shape;
		shape._mode = GL_TRIANGLE_STRIP;
		shape._vertices.push_back(texture_vertex(glm::vec2(-1,  1), glm::vec2(0, 1)));
		shape._vertices.push_back(texture_vertex(glm::vec2(-1, -1), glm::vec2(0, 0)));
		shape._vertices.push_back(texture_vertex(glm::vec2( 1,  1), glm::vec2(1, 1)));
		shape._vertices.push_back(texture_vertex(glm::vec2( 1, -1), glm::vec2(1, 0)));

		sobel_uniforms su;
		su._transform = glm::mat4x4();
		su._depth = _depth;
		_renderers->render_sobel_filter(shape, su);

		glDepthMask(true);
		glEnable(GL_DEPTH_TEST);
	}
}


void SmoothTerrainSurfaceRenderer::UpdateHeights()
{
	bounds2f bounds = _terrainSurfaceModel->GetBounds();
	glm::vec2 min = bounds.min;
	glm::vec2 size = bounds.size();

	int n = _size - 1;
	float k = n;
	int index = 0;
	for (int y = 0; y <= n; ++y)
	{
		for (int x = 0; x <= n; ++x)
		{
			float kx = x / k;
			float ky = y / k;
			glm::vec2 p = min + glm::vec2(kx * size.x, ky * size.y);

			_heights[index++] = _terrainSurfaceModel->CalculateHeight(p);
		}
	}
}


void SmoothTerrainSurfaceRenderer::UpdateNormals()
{
	bounds2f bounds = GetSourceBounds();
	glm::vec2 min = bounds.min;
	glm::vec2 size = bounds.size();

	int n = _size - 1;
	float k = n;
	glm::vec2 delta = 2.0f * size / k;
	int index = 0;
	for (int y = 0; y < _size; ++y)
	{
		for (int x = 0; x < _size; ++x)
		{
			int index_xn = x != 0 ? index - 1 : index;
			int index_xp = x != n ? index + 1 : index;
			int index_yn = y != 0 ? index - _size : index;
			int index_yp = y != n ? index + _size : index;

			float delta_hx = _heights[index_xp] - _heights[index_xn];
			float delta_hy = _heights[index_yp] - _heights[index_yn];

			glm::vec3 v1 = glm::vec3(delta.x, 0, delta_hx);
			glm::vec3 v2 = glm::vec3(0, delta.y, delta_hy);

			_normals[index++] = glm::normalize(glm::cross(v1, v2));
		}
	}
}


float SmoothTerrainSurfaceRenderer::InterpolateHeight(glm::vec2 position) const
{
	bounds2f bounds = GetSourceBounds();
	glm::vec2 p = (position - bounds.min) / bounds.size();
	bounds1i limit(0, _size - 1);

	int x = limit.clamp((int)glm::round(p.x * _size));
	int y = limit.clamp((int)glm::round(p.y * _size));

	return GetHeight(x, y);
}


glm::vec3 SmoothTerrainSurfaceRenderer::InterpolateNormal(glm::vec2 position) const
{
	bounds2f bounds = GetSourceBounds();
	glm::vec2 p = (position - bounds.min) / bounds.size();
	bounds1i limit(0, _size - 1);

	int x = limit.clamp((int)glm::round(p.x * _size));
	int y = limit.clamp((int)glm::round(p.y * _size));

	return GetNormal(x, y);
}


void SmoothTerrainSurfaceRenderer::UpdateDepthTextureSize()
{
	if (_depth != nullptr)
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		if (viewport[2] != _framebuffer_width || viewport[3] != _framebuffer_height)
		{
			_framebuffer_width = viewport[2];
			_framebuffer_height = viewport[3];

			glBindTexture(GL_TEXTURE_2D, _depth->id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _framebuffer_width, _framebuffer_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);

			if (_colorbuffer != nullptr)
				_colorbuffer->resize(GL_RGBA, _framebuffer_width, _framebuffer_height);
		}
	}
}



void SmoothTerrainSurfaceRenderer::InitializeShadow()
{
	bounds2f bounds = GetSourceBounds();
	glm::vec2 center = bounds.center();
	float radius1 = bounds.width() / 2;
	float radius2 = radius1 * 1.075f;

	_vboShadow._mode = GL_TRIANGLES;
	_vboShadow._vertices.clear();

	int n = 16;
	for (int i = 0; i < n; ++i)
	{
		float angle1 = i * 2 * (float)M_PI / n;
		float angle2 = (i + 1) * 2 * (float)M_PI / n;

		glm::vec2 p1 = center + radius1 * vector2_from_angle(angle1);
		glm::vec2 p2 = center + radius2 * vector2_from_angle(angle1);
		glm::vec2 p3 = center + radius2 * vector2_from_angle(angle2);
		glm::vec2 p4 = center + radius1 * vector2_from_angle(angle2);

		_vboShadow._vertices.push_back(plain_vertex(p1));
		_vboShadow._vertices.push_back(plain_vertex(p2));
		_vboShadow._vertices.push_back(plain_vertex(p3));
		_vboShadow._vertices.push_back(plain_vertex(p3));
		_vboShadow._vertices.push_back(plain_vertex(p4));
		_vboShadow._vertices.push_back(plain_vertex(p1));
	}

	_vboShadow.update(GL_STATIC_DRAW);
}



void SmoothTerrainSurfaceRenderer::InitializeSkirt()
{
	bounds2f bounds = GetSourceBounds();
	glm::vec2 center = bounds.center();
	float radius = bounds.width() / 2;

	_vboSkirt._mode = GL_TRIANGLE_STRIP;
	_vboSkirt._vertices.clear();

	int n = 256;
	float d = 2 * (float)M_PI / n;
	for (int i = 0; i < n; ++i)
	{
		float a = d * i;
		glm::vec2 p = center + radius * vector2_from_angle(a);
		float h = fmaxf(0, InterpolateHeight(p));

		_vboSkirt._vertices.push_back(skirt_vertex(glm::vec3(p, h), h));
		_vboSkirt._vertices.push_back(skirt_vertex(glm::vec3(p, -2.5), h));
	}

	_vboSkirt._vertices.push_back(_vboSkirt._vertices[0]);
	_vboSkirt._vertices.push_back(_vboSkirt._vertices[1]);

	_vboSkirt.update(GL_STATIC_DRAW);
}



void SmoothTerrainSurfaceRenderer::UpdateSplatmap()
{
	const image& map = *_terrainSurfaceModel->GetGroundtMap();

	glm::ivec2 size = map.size();
	if (_splatmapImage == nullptr)
		_splatmapImage = new image(size.x, size.y);

	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
		{
			glm::vec4 c = map.get_pixel(x, y);
			glm::vec2 p = _terrainSurfaceModel->MapImageToWorld(glm::ivec2(x, y));
			c.r = _terrainSurfaceModel->IsImpassable(p) ? 1.0f : 0.0f;
			_splatmapImage->set_pixel(x, y, c);
		}

	_splatmap->load(*_splatmapImage);
}



void SmoothTerrainSurfaceRenderer::UpdateChanges(bounds2f bounds)
{
	UpdateHeights();
	UpdateNormals();

	InitializeSkirt();
	UpdateSplatmap();

	// inside
	for (terrain_vertex& vertex : _vboInside._vertices)
	{
		glm::vec2 p = vertex._position.xy();
		if (bounds.contains(p))
		{
			vertex._position.z = GetHeight(vertex._x, vertex._y);
			vertex._normal = GetNormal(vertex._x, vertex._y);
		}
	}
	_vboInside.update(GL_STATIC_DRAW);

	// border
	for (terrain_vertex& vertex : _vboBorder._vertices)
	{
		glm::vec2 p = vertex._position.xy();
		if (bounds.contains(p))
		{
			vertex._position.z = GetHeight(vertex._x, vertex._y);
			vertex._normal = GetNormal(vertex._x, vertex._y);
		}
	}
	_vboBorder.update(GL_STATIC_DRAW);

	// lines
	for (color_vertex3& vertex : _vboLines._vertices)
	{
		glm::vec2 p = vertex._position.xy();
		if (bounds.contains(p))
		{
			vertex._position.z = InterpolateHeight(p);
		}
	}
	_vboLines.update(GL_STATIC_DRAW);

	// skirt
	for (size_t i = 0; i < _vboSkirt._vertices.size(); i += 2)
	{
		glm::vec2 p = _vboSkirt._vertices[i]._position.xy();
		if (bounds.contains(p))
		{
			float h = fmaxf(0, InterpolateHeight(p));
			_vboSkirt._vertices[i]._height = h;
			_vboSkirt._vertices[i]._position.z = h;
		}
	}
	_vboSkirt.update(GL_STATIC_DRAW);
}



void SmoothTerrainSurfaceRenderer::InitializeLines()
{
	bounds2f bounds = GetSourceBounds();
	glm::vec2 corner = bounds.min;
	glm::vec2 size = bounds.size();

	glm::vec4 black(0, 0, 0, 0.06f);

	_vboLines._mode = GL_LINES;
	_vboLines._vertices.clear();
	int n = _size - 1;
	float k = n;
	for (int x = 0; x <= n; x += 2)
		for (int y = 0; y <= n; y += 2)
		{
			float x0 = corner.x + size.x * (x / k);
			float y0 = corner.y + size.y * (y / k);
			float h00 = GetHeight(x, y);

			float x2, h20;
			if (x != n)
			{
				x2 = corner.x + size.x * ((x + 2) / k);
				h20 = GetHeight(x + 2, y);
				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x0, y0, h00), black));
				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x2, y0, h20), black));
			}
			float y2, h02;
			if (y != n)
			{
				y2 = corner.y + size.y * ((y + 2) / k);
				h02 = GetHeight(x, y + 2);
				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x0, y0, h00), black));
				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x0, y2, h02), black));
			}

			if (x != n && y != n)
			{
				float x1 = corner.x + size.x * ((x + 1) / k);
				float y1 = corner.y + size.y * ((y + 1) / k);
				float h11 = GetHeight(x + 1, y + 1);
				float h22 = GetHeight(x + 2, y + 2);

				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x0, y0, h00), black));
				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x1, y1, h11), black));

				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x2, y0, h20), black));
				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x1, y1, h11), black));

				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x0, y2, h02), black));
				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x1, y1, h11), black));

				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x2, y2, h22), black));
				_vboLines._vertices.push_back(color_vertex3(glm::vec3(x1, y1, h11), black));
			}
		}
	_vboLines.update(GL_STATIC_DRAW);
}


static int inside_circle(bounds2f bounds, glm::vec2 p)
{
	return glm::length(p - bounds.center()) <= bounds.width() / 2 ? 1 : 0;
}


static int inside_circle(bounds2f bounds, terrain_vertex v1, terrain_vertex v2, terrain_vertex v3)
{
	return inside_circle(bounds, v1._position.xy())
		+ inside_circle(bounds, v2._position.xy())
		+ inside_circle(bounds, v3._position.xy());

}



void SmoothTerrainSurfaceRenderer::BuildTriangles()
{
	bounds2f bounds = GetSourceBounds();
	glm::vec2 corner = bounds.p11();
	glm::vec2 size = bounds.size();

	_vboInside._mode = GL_TRIANGLES;
	_vboInside._vertices.clear();

	_vboBorder._mode = GL_TRIANGLES;
	_vboBorder._vertices.clear();

	int n = _size - 1;
	float k = n;

	for (int x = 0; x < n; x += 2)
		for (int y = 0; y < n; y += 2)
		{
			float x0 = corner.x + size.x * (x / k);
			float x1 = corner.x + size.x * ((x + 1) / k);
			float x2 = corner.x + size.x * ((x + 2) / k);
			float y0 = corner.y + size.y * (y / k);
			float y1 = corner.y + size.y * ((y + 1) / k);
			float y2 = corner.y + size.y * ((y + 2) / k);

			float h00 = GetHeight(x, y);
			float h02 = GetHeight(x, y + 2);
			float h20 = GetHeight(x + 2, y);
			float h11 = GetHeight(x + 1, y + 1);
			float h22 = GetHeight(x + 2, y + 2);

			glm::vec3 n00 = GetNormal(x, y);
			glm::vec3 n02 = GetNormal(x, y + 2);
			glm::vec3 n20 = GetNormal(x + 2, y);
			glm::vec3 n11 = GetNormal(x + 1, y + 1);
			glm::vec3 n22 = GetNormal(x + 2, y + 2);

			terrain_vertex v00 = terrain_vertex(x + 0, y + 0, glm::vec3(x0, y0, h00), n00);
			terrain_vertex v02 = terrain_vertex(x + 0, y + 2, glm::vec3(x0, y2, h02), n02);
			terrain_vertex v20 = terrain_vertex(x + 2, y + 0, glm::vec3(x2, y0, h20), n20);
			terrain_vertex v11 = terrain_vertex(x + 1, y + 1, glm::vec3(x1, y1, h11), n11);
			terrain_vertex v22 = terrain_vertex(x + 2, y + 2, glm::vec3(x2, y2, h22), n22);

			PushTriangle(bounds, v00, v20, v11);
			PushTriangle(bounds, v20, v22, v11);
			PushTriangle(bounds, v22, v02, v11);
			PushTriangle(bounds, v02, v00, v11);
		}

	_vboInside.update(GL_STATIC_DRAW);
	_vboBorder.update(GL_STATIC_DRAW);
}


void SmoothTerrainSurfaceRenderer::PushTriangle(const bounds2f& bounds, const terrain_vertex& v0, const terrain_vertex& v1, const terrain_vertex& v2)
{
	vertexbuffer<terrain_vertex>* s = SelectTerrainVbo(inside_circle(bounds, v0, v1, v2));
	if (s != nullptr)
	{
		s->_vertices.push_back(v0);
		s->_vertices.push_back(v1);
		s->_vertices.push_back(v2);
	}
}


vertexbuffer<terrain_vertex>* SmoothTerrainSurfaceRenderer::SelectTerrainVbo(int inside)
{
	switch (inside)
	{
		case 1:
		case 2:
			return &_vboBorder;
		case 3:
			return &_vboInside;
		default:
			return nullptr;
	}
}
