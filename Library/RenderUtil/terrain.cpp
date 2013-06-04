/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "terrain.h"
#include "image.h"



static void render_terrain_inside(shape<terrain_vertex>& shape, const terrain_uniforms& uniforms)
{
	static renderer<terrain_vertex, terrain_uniforms>* _renderer = nullptr;
	if (_renderer == nullptr)
	{
		_renderer = new renderer<terrain_vertex, terrain_uniforms>((
			VERTEX_ATTRIBUTE(terrain_vertex, _position),
			VERTEX_ATTRIBUTE(terrain_vertex, _normal),
			SHADER_UNIFORM(terrain_uniforms, _transform),
			SHADER_UNIFORM(terrain_uniforms, _light_normal),
			SHADER_UNIFORM(terrain_uniforms, _colors),
			SHADER_UNIFORM(terrain_uniforms, _forest),
			VERTEX_SHADER
			({
				uniform mat4 transform;
				uniform vec3 light_normal;

				attribute vec3 position;
				attribute vec3 normal;

				varying vec3 _position;
				varying vec2 _terraincoord;
				varying vec2 _colorcoord;
				//varying float _normal_z;
				varying float _brightness;

				void main()
				{
					vec4 p = transform * vec4(position, 1);

					float brightness = -dot(light_normal, normal);

					_position = position;
					_terraincoord = position.xy / 1024.0;
					_colorcoord = vec2(brightness, 1.0 - (2.5 + position.z) / 128.0);
					//_normal_z = normal.z;
					_brightness = brightness;


				    gl_Position = p;
					gl_PointSize = 1.0;
				}
			}),
			FRAGMENT_SHADER
			({
				uniform sampler2D colors;
				uniform sampler2D forest;
				uniform sampler2D cliffs;
				uniform sampler2D grass;

				varying vec3 _position;
				varying vec2 _terraincoord;
				varying vec2 _colorcoord;
				//varying float _normal_z;
				varying float _brightness;

				void main()
				{
					vec3 color = texture2D(colors, _colorcoord).rgb;

					float f = step(0.0, _position.z) * smoothstep(0.475, 0.525, texture2D(forest, _terraincoord).r);
					color = mix(color, vec3(0.2196, 0.3608, 0.1922), 0.3 * f);
					color = mix(color, vec3(0), 0.03 * step(0.5, 1.0 - _brightness));

				    gl_FragColor = vec4(color, 1.0);
				}
			})
		));
		_renderer->_blend_sfactor = GL_ONE;
		_renderer->_blend_dfactor = GL_ZERO;
	}
	_renderer->render(shape, uniforms);
}



static void render_terrain_border(shape<terrain_vertex>& shape, const terrain_uniforms& uniforms)
{
	static renderer<terrain_vertex, terrain_uniforms>* _renderer = nullptr;
	if (_renderer == nullptr)
	{
		_renderer = new renderer<terrain_vertex, terrain_uniforms>((
			VERTEX_ATTRIBUTE(terrain_vertex, _position),
			VERTEX_ATTRIBUTE(terrain_vertex, _normal),
			SHADER_UNIFORM(terrain_uniforms, _transform),
			SHADER_UNIFORM(terrain_uniforms, _light_normal),
			SHADER_UNIFORM(terrain_uniforms, _colors),
			SHADER_UNIFORM(terrain_uniforms, _forest),
			VERTEX_SHADER
			({
				uniform mat4 transform;
				uniform vec3 light_normal;

				attribute vec3 position;
				attribute vec3 normal;

				varying vec3 _position;
				varying vec2 _terraincoord;
				varying vec2 _colorcoord;
				//varying float _normal_z;
				varying float _brightness;

				void main()
				{
					vec4 p = transform * vec4(position, 1);

					float brightness = -dot(light_normal, normal);

					_position = position;
					_terraincoord = position.xy / 1024.0;
					_colorcoord = vec2(brightness, 1.0 - (2.5 + position.z) / 128.0);
					//_normal_z = normal.z;
					_brightness = brightness;


				    gl_Position = p;
					gl_PointSize = 1.0;
				}
			}),
			FRAGMENT_SHADER
			({
				uniform sampler2D colors;
				uniform sampler2D forest;
				uniform sampler2D cliffs;
				uniform sampler2D grass;

				varying vec3 _position;
				varying vec2 _terraincoord;
				varying vec2 _colorcoord;
				//varying float _normal_z;
				varying float _brightness;

				void main()
				{
					if (distance(_terraincoord, vec2(0.5, 0.5)) > 0.5)
						discard;

					vec3 color = texture2D(colors, _colorcoord).rgb;

					float f = step(0.0, _position.z) * smoothstep(0.475, 0.525, texture2D(forest, _terraincoord).r);
					color = mix(color, vec3(0.2196, 0.3608, 0.1922), 0.3 * f);
					color = mix(color, vec3(0), 0.03 * step(0.5, 1.0 - _brightness));

				    gl_FragColor = vec4(color, 1.0);
				}
			})
		));
		_renderer->_blend_sfactor = GL_ONE;
		_renderer->_blend_dfactor = GL_ZERO;
	}
	_renderer->render(shape, uniforms);
}



static void render_terrain_edge(shape<terrain_edge_vertex>& shape, const texture_uniforms& uniforms)
{
	static renderer<terrain_edge_vertex, texture_uniforms>* _renderer = nullptr;
	if (_renderer == nullptr)
	{
		_renderer = new renderer<terrain_edge_vertex, texture_uniforms>((
			VERTEX_ATTRIBUTE(terrain_edge_vertex, _position),
			VERTEX_ATTRIBUTE(terrain_edge_vertex, _height),
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
		_renderer->_blend_sfactor = GL_ONE;
		_renderer->_blend_dfactor = GL_ZERO;
	}
	_renderer->render(shape, uniforms);
}



static void render_depth_inside(shape<terrain_vertex>& shape, const terrain_uniforms& uniforms)
{
	static renderer<terrain_vertex, terrain_uniforms>* _renderer = nullptr;
	if (_renderer == nullptr)
	{
		_renderer = new renderer<terrain_vertex, terrain_uniforms>((
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
		_renderer->_blend_sfactor = GL_ONE;
		_renderer->_blend_dfactor = GL_ZERO;
	}
	_renderer->render(shape, uniforms);
}



static void render_depth_border(shape<terrain_vertex>& shape, const terrain_uniforms& uniforms)
{
	static renderer<terrain_vertex, terrain_uniforms>* _renderer = nullptr;
	if (_renderer == nullptr)
	{
		_renderer = new renderer<terrain_vertex, terrain_uniforms>((
			VERTEX_ATTRIBUTE(terrain_vertex, _position),
			VERTEX_ATTRIBUTE(terrain_vertex, _normal),
			SHADER_UNIFORM(terrain_uniforms, _transform),
			VERTEX_SHADER
			({
				uniform mat4 transform;
				attribute vec3 position;
				attribute vec3 normal;
				varying vec2 _terraincoord;

				void main()
				{
					_terraincoord = position.xy / 1024.0;
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
		_renderer->_blend_sfactor = GL_ONE;
		_renderer->_blend_dfactor = GL_ZERO;
	}
	_renderer->render(shape, uniforms);
}



static void render_depth_edge(shape<terrain_edge_vertex>& shape, const plain_uniforms& uniforms)
{
	static renderer<terrain_edge_vertex, plain_uniforms>* _renderer = nullptr;
	if (_renderer == nullptr)
	{
		_renderer = new renderer<terrain_edge_vertex, plain_uniforms>((
			VERTEX_ATTRIBUTE(terrain_edge_vertex, _position),
			VERTEX_ATTRIBUTE(terrain_edge_vertex, _height),
			SHADER_UNIFORM(plain_uniforms, _transform),
			VERTEX_SHADER
			({
				attribute vec3 position;
				attribute float height;
				uniform mat4 transform;

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
		_renderer->_blend_sfactor = GL_ONE;
		_renderer->_blend_dfactor = GL_ZERO;
	}
	_renderer->render(shape, uniforms);
}



static void render_sobel(shape<texture_vertex>& shape, const sobel_uniforms& uniforms)
{
	static renderer<texture_vertex, sobel_uniforms>* _renderer = nullptr;
	if (_renderer == nullptr)
	{
		_renderer = new renderer<texture_vertex, sobel_uniforms>((
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
		_renderer->_blend_sfactor = GL_SRC_ALPHA;
		_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
	}
	_renderer->render(shape, uniforms);
}





static glm::vec3 heightcolor(float h)
{
	static std::vector<std::pair<float, glm::vec3>>* _colors = nullptr;
	if (_colors == nullptr)
	{
		_colors = new std::vector<std::pair<float, glm::vec3>>();
		_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(164, 146, 124) / 255.0f));
		_colors->push_back(std::pair<float, glm::vec3>(-0.5, glm::vec3(219, 186, 153) / 255.0f));
		_colors->push_back(std::pair<float, glm::vec3>(0.0,  glm::vec3(194, 142, 102) / 255.0f));
		_colors->push_back(std::pair<float, glm::vec3>(6.5,  glm::vec3(199, 172, 148) / 255.0f));
		_colors->push_back(std::pair<float, glm::vec3>(7.0,  glm::vec3(177, 172, 132) / 255.0f));
		_colors->push_back(std::pair<float, glm::vec3>(10,   glm::vec3(125, 171, 142) / 255.0f));
		_colors->push_back(std::pair<float, glm::vec3>(50,   glm::vec3(59,  137,  11) / 255.0f));
		_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(59,  137,  11) / 255.0f));
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



static texture* create_colors()
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
			glm::vec3 c = adjust_brightness(heightcolor(h), brightness);
			if (h > 0)
				c = glm::mix(c, r[y], 0.015f);

			img.set_pixel(x, 255 - y, glm::vec4(c, 1));
		}

	return new texture(img);
}



terrain::terrain(heightmap* height, image* forest, bool render_edges) :
_framebuffer_width(0),
_framebuffer_height(0),
_framebuffer(nullptr),
_depth(nullptr),
_colors(nullptr),
_forest(nullptr),
_heightmap(height)
{
	if (render_edges)
	{
		_depth = new texture();
		glBindTexture(GL_TEXTURE_2D, _depth->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		update_depth_texture_size();

		_framebuffer = new framebuffer();
		_framebuffer->attach_depth(_depth);
		{
			bind_framebuffer binding(*_framebuffer);
			//CHECK_GL_FRAMEBUFFER();
		}
	}

	_colors = create_colors();
	glBindTexture(GL_TEXTURE_2D, _colors->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	_forest = new texture(*forest);


	load_chunk(terrain_address(), 0);

	initialize_edge();
}



terrain::~terrain()
{
	for (std::pair<terrain_address, terrain_chunk*> i : _chunks)
		delete i.second;

	delete _colors;
	delete _forest;
	delete _framebuffer;
	delete _depth;
}



void terrain::update_heights(bounds2f bounds)
{
	for (std::pair<terrain_address, terrain_chunk*> iter : _chunks)
	{
		terrain_chunk* chunk = iter.second;

		for (terrain_vertex& vertex : chunk->_inside._vertices)
		{
			glm::vec2 p = vertex._position.xy();
			if (bounds.contains(p))
			{
				vertex._position.z = _heightmap->get_height(p);
				vertex._normal = _heightmap->get_normal(p);
			}
		}
		chunk->_inside.update(GL_STATIC_DRAW);

		for (terrain_vertex& vertex : chunk->_border._vertices)
		{
			glm::vec2 p = vertex._position.xy();
			if (bounds.contains(p))
			{
				vertex._position.z = _heightmap->get_height(p);
				vertex._normal = _heightmap->get_normal(p);
			}
		}
		chunk->_border.update(GL_STATIC_DRAW);

		for (size_t i = 0; i < _shape_terrain_edge._vertices.size(); i += 2)
		{
			glm::vec2 p = _shape_terrain_edge._vertices[i]._position.xy();
			if (bounds.contains(p))
			{
				float h = fmaxf(0, _heightmap->get_height(p)) + 0.25f;
				_shape_terrain_edge._vertices[i]._height = h;
				_shape_terrain_edge._vertices[i]._position.z = h;
			}
		}
		_shape_terrain_edge.update(GL_STATIC_DRAW);
	}
}



void terrain::update_depth_texture_size()
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
		}
	}
}



void terrain::initialize_edge()
{
	_shape_terrain_edge._mode = GL_TRIANGLE_STRIP;
	_shape_terrain_edge._vertices.clear();

	int n = 256;
	float d = 2 * (float)M_PI / n;
	for (int i = 0; i < n; ++i)
	{
		float a = d * i;
		glm::vec2 p = 512.01f * vector2_from_angle(a) + 512.0f;
		float h = fmaxf(0, _heightmap->get_height(p)) + 0.25f;

		_shape_terrain_edge._vertices.push_back(terrain_edge_vertex(glm::vec3(p, h), h));
		_shape_terrain_edge._vertices.push_back(terrain_edge_vertex(glm::vec3(p, -2.5), h));
	}

	_shape_terrain_edge._vertices.push_back(_shape_terrain_edge._vertices[0]);
	_shape_terrain_edge._vertices.push_back(_shape_terrain_edge._vertices[1]);

	_shape_terrain_edge.update(GL_STATIC_DRAW);
}



void terrain::render(const terrain_uniforms& uniforms)
{
	if (_framebuffer != nullptr)
	{
		update_depth_texture_size();

		bind_framebuffer binding(*_framebuffer);

		glClear(GL_DEPTH_BUFFER_BIT);

		terrain_uniforms du;
		du._transform = uniforms._transform;

		foreach_leaf(terrain_address(), [du](terrain_chunk& s) {
			render_depth_inside(s._inside, du);
			render_depth_border(s._border, du);
		});

		plain_uniforms pu;
		pu._transform = uniforms._transform;
		render_depth_edge(_shape_terrain_edge, pu);
	}

	foreach_leaf(terrain_address(), [uniforms](terrain_chunk& s) {
		render_terrain_inside(s._inside, uniforms);
		render_terrain_border(s._border, uniforms);

		/*glDisable(GL_DEPTH_TEST);
		gradient_uniforms g;
		g._transform = uniforms._transform;
		renderers::_gradient_renderer3->render(s._lines, g);
		glEnable(GL_DEPTH_TEST);*/
	});

	texture_uniforms tu;
	tu._transform = uniforms._transform;
	tu._texture = _colors;
	render_terrain_edge(_shape_terrain_edge, tu);

	if (_depth != nullptr)
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);

		shape<texture_vertex> shape;
		shape._mode = GL_TRIANGLE_STRIP;
		shape._vertices.push_back(texture_vertex(glm::vec2(-1,  1), glm::vec2(0, 1)));
		shape._vertices.push_back(texture_vertex(glm::vec2(-1, -1), glm::vec2(0, 0)));
		shape._vertices.push_back(texture_vertex(glm::vec2( 1,  1), glm::vec2(1, 1)));
		shape._vertices.push_back(texture_vertex(glm::vec2( 1, -1), glm::vec2(1, 0)));

		sobel_uniforms su;
		su._transform = glm::mat4x4();
		su._depth = _depth;
		render_sobel(shape, su);

		glDepthMask(true);
		glEnable(GL_DEPTH_TEST);
	}
}



void terrain::foreach_leaf(terrain_address chunk, std::function<void(terrain_chunk&)> f)
{
	if (_split.find(chunk) != _split.end())
	{
		chunk.foreach_child([this, f](terrain_address child) {
			foreach_leaf(child, f);
		});
	}
	else if (_chunks.find(chunk) != _chunks.end())
	{
		f(*_chunks[chunk]);
	}
}



bool terrain::is_loaded(terrain_address chunk)
{
	return _chunks.find(chunk) != _chunks.end();
}



void terrain::load_chunk(terrain_address chunk, float priority)
{
	if (_chunks.find(chunk) == _chunks.end())
		_chunks[chunk] = create_node(chunk);
}


void terrain::unload_chunk(terrain_address chunk)
{
	_chunks.erase(chunk);
}




void terrain::load_children(terrain_address chunk, float priority)
{
	if (priority < 0.5f)
	{
		request_unload_children(chunk);
	}
	else
	{
		request_load_children_unload_grand_children(chunk, priority);
	}
}



terrain_chunk* terrain::create_node(terrain_address chunk)
{
	terrain_chunk* result = new terrain_chunk(chunk);

	result->_parent = chunk._level != 0 ? _chunks[chunk.get_parent()] : nullptr;
	result->_bounds = get_bounds(chunk);

	build_lines(result->_lines, chunk);
	build_triangles(result);

	return result;
}






void terrain::request_load_children_unload_grand_children(terrain_address chunk, float priority)
{
	chunk.foreach_child([this, priority](terrain_address child) {
		if (!is_loaded(child))
			load_chunk(child, priority);

		request_unload_children(child);
	});
}



void terrain::request_unload_children(terrain_address chunk)
{
	chunk.foreach_child([this](terrain_address child) {
		unload_chunk(child);
	});
}



bool terrain::is_split(terrain_address chunk)
{
	return _split.find(chunk) != _split.end();
}



bool terrain::can_chunk_be_splitted(terrain_address chunk)
{
	if (is_split(chunk))
		return true; // already split

	if (!chunk.all_children([this](terrain_address child) {
		return is_loaded(child);
	}))
		return false;

	if (!chunk.all_ancestors([this](terrain_address child) {
		return can_chunk_be_splitted(child);
	}))
		return false;

	//if (!chunk.GetNeighbors().All(x => CanGrandParentBeSplitted(x)))
	//	return false;

	return true;
}



bool terrain::can_grand_parent_be_splitted(terrain_address chunk)
{
	return chunk._level < 2 ? false : can_chunk_be_splitted(chunk.get_parent().get_parent());
}



void terrain::set_split(terrain_address chunk)
{
	if (_split.find(chunk) == _split.end())
		_split[chunk] = true;

	chunk.foreach_ancestor([this](terrain_address ancestor) {
		set_split(ancestor);
	});
}



void terrain::clear_split(terrain_address chunk)
{
	std::vector<terrain_address> s;
	for (auto i : _split)
		s.push_back(i.first);

	for (terrain_address c : s)
	{
		bool is_ancestor = false;
		c.foreach_ancestor([chunk, &is_ancestor](terrain_address ancestor) {
			if (ancestor == chunk) is_ancestor = true;
		});
		if (is_ancestor)
			_split.erase(c);
	}

	if (_split.find(chunk) != _split.end())
		_split.erase(chunk);
}



void terrain::set_lod(terrain_address chunk, float lod)
{
	_lod[chunk] = lod;
}



float terrain::get_lod(terrain_address chunk)
{
	if (_lod.find(chunk) != _lod.end())
		return _lod[chunk];
	return 0;
}



bounds3f terrain::get_bounds(terrain_address chunk) const
{
	glm::vec2 size = _heightmap->get_bounds().size() / (float)(1 << chunk._level);
	glm::vec2 corner = _heightmap->get_bounds().min + size * glm::vec2(chunk._x, chunk._y);

	glm::vec3 min = glm::vec3(corner, 0);
	glm::vec3 max = glm::vec3(corner + size, _heightmap->max_height());

	return bounds3f(min, max);
}



void terrain::build_lines(shape<color_vertex3>& shape, terrain_address chunk)
{
	bounds2f bounds = get_bounds(chunk).xy();
	glm::vec2 corner = bounds.p11();
	glm::vec2 size = bounds.size();

	glm::vec4 black(0, 0, 0, 0.2f);

	float d = 0.005f;
	int nx = 20;
	int ny = 20;

	shape._mode = GL_LINES;
	shape._vertices.clear();
	for (int x = 0; x <= nx; ++x)
		for (int y = 0; y <= ny; ++y)
		{
			float x1 = corner.x + size.x * x / nx;
			float x2 = corner.x + size.x * (x + 1) / nx;
			float y1 = corner.y + size.y * y / ny;
			float y2 = corner.y + size.y * (y + 1) / ny;

			if (y != ny)
			{
				shape._vertices.push_back(color_vertex3(glm::vec3(x1, y1, _heightmap->get_height(glm::vec2(x1, y1)) + d), black));
				shape._vertices.push_back(color_vertex3(glm::vec3(x1, y2, _heightmap->get_height(glm::vec2(x1, y2)) + d), black));
			}
			if (x != nx)
			{
				shape._vertices.push_back(color_vertex3(glm::vec3(x1, y1, _heightmap->get_height(glm::vec2(x1, y1)) + d), black));
				shape._vertices.push_back(color_vertex3(glm::vec3(x2, y1, _heightmap->get_height(glm::vec2(x2, y1)) + d), black));
			}
		}
	shape.update(GL_STATIC_DRAW);
}



static int inside_circle(glm::vec2 p)
{
	return glm::length(p - glm::vec2(512, 512)) <= 512 ? 1 : 0;
}


static int inside_circle(terrain_vertex v1, terrain_vertex v2, terrain_vertex v3)
{
	return inside_circle(v1._position.xy())
		+ inside_circle(v2._position.xy())
		+ inside_circle(v3._position.xy());

}



void terrain::build_triangles(terrain_chunk* chunk)
{
	bounds2f bounds = get_bounds(chunk->_address).xy();
	glm::vec2 corner = bounds.p11();
	glm::vec2 size = bounds.size();

	int nx = 20;
	int ny = 20;

	chunk->_inside._mode = GL_TRIANGLES;
	chunk->_inside._vertices.clear();

	chunk->_border._mode = GL_TRIANGLES;
	chunk->_border._vertices.clear();

	for (int x = 0; x < nx; ++x)
		for (int y = 0; y < ny; ++y)
		{
			float x1 = corner.x + size.x * x / nx;
			float x2 = corner.x + size.x * (x + 1) / nx;
			float y1 = corner.y + size.y * y / ny;
			float y2 = corner.y + size.y * (y + 1) / ny;

			terrain_vertex v11 = make_terrain_vertex(x1, y1);
			terrain_vertex v12 = make_terrain_vertex(x1, y2);
			terrain_vertex v21 = make_terrain_vertex(x2, y1);
			terrain_vertex v22 = make_terrain_vertex(x2, y2);

			shape<terrain_vertex>* s = chunk->triangle_shape(inside_circle(v11, v22, v12));
			if (s != nullptr)
			{
				s->_vertices.push_back(v11);
				s->_vertices.push_back(v22);
				s->_vertices.push_back(v12);
			}

			s = chunk->triangle_shape(inside_circle(v22, v11, v21));
			if (s != nullptr)
			{
				s->_vertices.push_back(v22);
				s->_vertices.push_back(v11);
				s->_vertices.push_back(v21);
			}
		}

	chunk->_inside.update(GL_STATIC_DRAW);
	chunk->_border.update(GL_STATIC_DRAW);
}



terrain_vertex terrain::make_terrain_vertex(float x, float y)
{
	glm::vec2 p = glm::vec2(x, y);
	float z = _heightmap->get_height(p);
	return terrain_vertex(glm::vec3(x, y, z), _heightmap->get_normal(p));
}


color_vertex3 terrain::make_color_vertex(float x, float y)
{
	float h = _heightmap->get_height(glm::vec2(x, y));
	float k = 0.7f + 0.25f * h / 60;
	glm::vec4 c(0.3f, k, 0.3f, 1.0f);
	return color_vertex3(glm::vec3(x, y, h), c);
}






/***/


terrain_viewpoint::terrain_viewpoint(terrain* terrain) : _terrain(terrain)
{
}



void terrain_viewpoint::set_parameters(
	float errorLodMax,
	float maxPixelError,
	float screenWidth,
	float horizontalFOVDegrees)
{
	float tanHalfFOV = tanf(0.5f * horizontalFOVDegrees * (float)M_PI / 180.0f);
	float K = screenWidth / tanHalfFOV;

	_distance_lod_max = (errorLodMax / maxPixelError) * K;

	_near = 0.0;
	_far = 5;
	_near_lod = 2;
}



float terrain_viewpoint::compute_lod(bounds3f boundingBox) const
{
	glm::vec3 center = (boundingBox.max + boundingBox.min) * 0.5f;
	glm::vec3 extent = (boundingBox.max - boundingBox.min) * 0.5f;

	glm::vec3 distance = _viewpoint - center;
	distance.x = fmaxf(0, fabsf(distance.x) - extent.x);
	distance.y = fmaxf(0, fabsf(distance.y) - extent.y);
	distance.z = fmaxf(0, fabsf(distance.z) - extent.z);

	return compute_lod(glm::length(distance));
}



float terrain_viewpoint::compute_lod(float distance) const
{
	float d = 1 + fminf(fmaxf(distance - _near, 0) / (_far - _near), 1);
	float x = log2f(d);
	return (1 - x) * _near_lod;
}



void terrain_viewpoint::update()
{
	if (_terrain->is_split(terrain_address()))
		_terrain->clear_split(terrain_address());

	update(terrain_address());
}



void terrain_viewpoint::update(terrain_address chunk)
{
	if (!_terrain->is_loaded(chunk))
	{
		_terrain->load_chunk(chunk, 1.0f);
		return;
	}

	if (!chunk.all_children([this](terrain_address x) { return _terrain->is_loaded(x); }))
	{
		chunk.foreach_child([this](terrain_address x) {
			if (!_terrain->is_loaded(x))
				_terrain->load_chunk(x, 1.0);
		});
		return;
	}

	bounds3f boundingBox = _terrain->get_bounds(chunk);
	float desiredLod = compute_lod(boundingBox);
	float lowerLod = chunk._level;
	float upperLod = chunk._level + 1;

	if (desiredLod >= upperLod && _terrain->can_chunk_be_splitted(chunk))
	{
		_terrain->set_split(chunk);

		chunk.foreach_child([this](terrain_address x) {
			update(x);
		});
	}
	else
	{
		_terrain->set_lod(chunk, desiredLod);

		float priority = desiredLod > lowerLod ? lowerLod : 0;

		_terrain->load_children(chunk, priority);
	}
}




/****/




terrain_address::terrain_address() : _level(0), _x(0), _y(0)
{
}



terrain_address::terrain_address(int level, int x, int y) : _level(level), _x(x), _y(y)
{
	/*if (level < 0 || level > 14)
		throw new ArgumentException(String.Format("level = {0} is invalid, must be 0 <= level <= 14 ", level), "level");
	if (x < 0 || x >= Math.Pow(2, level))
		throw new ArgumentException(String.Format("x = {0} is invalid, must be 0 <= x < {1}", x, (ushort)Math.Pow(2, level)), "x");
	if (z < 0 || z >= Math.Pow(2, level))
		throw new ArgumentException(String.Format("z = {0} is invalid, must be 0 <= z < {1}", x, (ushort)Math.Pow(2, level)), "z");*/
}



bool terrain_address::is_valid(int level, int x, int z)
{
	return 0 <= level && level <= 14
		&& 0 <= x && x < (1 << level)
		&& 0 <= z && z < (1 << level);
}



terrain_address terrain_address::get_parent()
{
	//if (_level == 0)
	//	throw new InvalidOperationException("root chunk has no parent");
	return terrain_address(_level - 1, _x / 2, _y / 2);
}



void terrain_address::foreach_neighbor(std::function<void(terrain_address)> action)
{
	if (is_valid(_level, _x - 1, _y)) action(terrain_address(_level, _x - 1, _y));
	if (is_valid(_level, _x + 1, _y)) action(terrain_address(_level, _x + 1, _y));
	if (is_valid(_level, _x, _y - 1)) action(terrain_address(_level, _x, _y - 1));
	if (is_valid(_level, _x, _y + 2)) action(terrain_address(_level, _x, _y + 1));
}



void terrain_address::foreach_child(std::function<void(terrain_address)> action)
{
	action(terrain_address(_level + 1, _x * 2, _y * 2));
	action(terrain_address(_level + 1, _x * 2 + 1, _y * 2));
	action(terrain_address(_level + 1, _x * 2, _y * 2 + 1));
	action(terrain_address(_level + 1, _x * 2 + 1, _y * 2 + 1));
}



void terrain_address::foreach_ancestor(std::function<void(terrain_address)> action)
{
	terrain_address chunk = *this;
	while (chunk._level != 0)
	{
		chunk = chunk.get_parent();
		action(chunk);
	}
}



bool terrain_address::all_children(std::function<bool (terrain_address)> predicate)
{
	return predicate(terrain_address(_level + 1, _x * 2, _y * 2))
		&& predicate(terrain_address(_level + 1, _x * 2 + 1, _y * 2))
		&& predicate(terrain_address(_level + 1, _x * 2, _y * 2 + 1))
		&& predicate(terrain_address(_level + 1, _x * 2 + 1, _y * 2 + 1));

}



bool terrain_address::all_ancestors(std::function<bool (terrain_address)> predicate)
{
	terrain_address chunk = *this;
	while (chunk._level != 0)
	{
		chunk = chunk.get_parent();
		if (!predicate(chunk))
			return false;
	}
	return true;
}



bool operator ==(terrain_address chunk1, terrain_address chunk2)
{
	return chunk1._level == chunk2._level
		&& chunk1._x == chunk2._x
		&& chunk1._y == chunk2._y;
}



bool operator !=(terrain_address chunk1, terrain_address chunk2)
{
	return chunk1._level != chunk2._level
		|| chunk1._x != chunk2._x
		|| chunk1._y != chunk2._y;
}



bool operator <(terrain_address chunk1, terrain_address chunk2)
{
	if (chunk1._level != chunk2._level)
		return chunk1._level < chunk2._level;
	if (chunk1._x != chunk2._x)
		return chunk1._x < chunk2._x;
	return chunk1._y < chunk2._y;
}



/***/



terrain_chunk::terrain_chunk(terrain_address address) : _address(address)
{
	_parent = nullptr;
	_neighbors[0] = nullptr;
	_neighbors[1] = nullptr;
	_neighbors[2] = nullptr;
	_neighbors[3] = nullptr;
	_children[0] = nullptr;
	_children[1] = nullptr;
	_children[2] = nullptr;
	_children[3] = nullptr;
	_is_split = false;
	_lod = 0;
}



bool terrain_chunk::has_children() const
{
	return _children[0] != nullptr;
}




shape<terrain_vertex>* terrain_chunk::triangle_shape(int inside)
{
	switch (inside)
	{
		case 1:
		case 2:
			return &_border;
		case 3:
			return &_inside;
		default:
			return nullptr;
	}
}
