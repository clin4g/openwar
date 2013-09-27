// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "../../Library/Algebra/image.h"
#include "SmoothTerrainSurface.h"



SmoothTerrainSurface::SmoothTerrainSurface(bounds2f bounds, image* groundmap) :
_bounds(bounds),
_groundmap(groundmap),
_framebuffer_width(0),
_framebuffer_height(0),
_framebuffer(nullptr),
_colorbuffer(nullptr),
_depth(nullptr),
_colormap(nullptr),
_splatmap(nullptr),
_size(255),
_heights(nullptr),
_normals(nullptr)
{
	if (groundmap->size() != glm::ivec2(256, 256))
		NSLog(@"SmoothTerrainSurface: ILLEGAL SIZE ############# %d x %d", groundmap->size().x, groundmap->size().y);

	_renderers = new terrain_renderers();
	_colormap = terrain_renderers::create_colormap();

	_heights = new float [_size * _size];
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


SmoothTerrainSurface::~SmoothTerrainSurface()
{
	delete _colormap;
	delete _splatmap;
	delete _framebuffer;
	delete _colorbuffer;
	delete _depth;
	delete _renderers;
}


float SmoothTerrainSurface::GetHeight(glm::vec2 position) const
{
	return InterpolateHeight(position);
}


const float* SmoothTerrainSurface::Intersect(ray r)
{
	glm::vec3 offset = glm::vec3(_bounds.min, 0);
	glm::vec3 scale = glm::vec3(glm::vec2(_size - 1, _size - 1) / _bounds.size(), 1);

	ray r2 = ray(scale * (r.origin - offset), glm::normalize(scale * r.direction));
	const float* d = InternalIntersect(r2);
	if (d == nullptr)
		return nullptr;

	static float result;
	result = glm::length((r2.point(*d) - r2.origin) / scale);
	return &result;
}


bool SmoothTerrainSurface::IsForest(glm::vec2 position) const
{
	glm::ivec2 coord = MapWorldToImage(position);
	return GetForestValue(coord.x, coord.y) >= 0.5;
}


bool SmoothTerrainSurface::IsImpassable(glm::vec2 position) const
{
	glm::ivec2 coord = MapWorldToImage(position);
	return GetImpassableValue(coord.x, coord.y) >= 0.5;
}


void SmoothTerrainSurface::Render(const glm::mat4x4& transform, const glm::vec3& lightNormal)
{
	glm::vec4 map_bounds = glm::vec4(_bounds.min, _bounds.size());

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
		shape._vertices.push_back(texture_vertex(glm::vec2(-1, 1), glm::vec2(0, 1)));
		shape._vertices.push_back(texture_vertex(glm::vec2(-1, -1), glm::vec2(0, 0)));
		shape._vertices.push_back(texture_vertex(glm::vec2(1, 1), glm::vec2(1, 1)));
		shape._vertices.push_back(texture_vertex(glm::vec2(1, -1), glm::vec2(1, 0)));

		sobel_uniforms su;
		su._transform = glm::mat4x4();
		su._depth = _depth;
		_renderers->render_sobel_filter(shape, su);

		glDepthMask(true);
		glEnable(GL_DEPTH_TEST);
	}
}


void SmoothTerrainSurface::Extract(glm::vec2 position, image* brush)
{
	glm::ivec2 size = brush->size();
	glm::ivec2 origin = MapWorldToImage(position) - size / 2;

	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
			brush->set_pixel(x, y, _groundmap->get_pixel(origin.x + x, origin.y + y));
}


bounds2f SmoothTerrainSurface::Paint(TerrainFeature feature, glm::vec2 position, image* brush, float pressure)
{
	glm::vec2 scale = _bounds.size() / glm::vec2(_groundmap->size());
	glm::ivec2 size = brush->size();
	glm::ivec2 center = MapWorldToImage(position);
	glm::ivec2 origin = center - size / 2;
	float radius = size.x / 2.0f;

	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
		{
			glm::ivec2 p = origin + glm::ivec2(x, y);
			float d = glm::distance(position, scale * glm::vec2(p)) / radius;
			float k = 1.0f - d * d;
			if (k > 0)
			{
				glm::vec4 b = brush->get_pixel(x, y);
				glm::vec4 c = _groundmap->get_pixel(p.x, p.y);
				switch (feature)
				{
					case TerrainFeature::Hills:
						c.a = glm::mix(c.a, b.a, k * pressure);
						break;
					case TerrainFeature::Trees:
						c.g = glm::mix(c.g, b.g, k * pressure);
						break;
					case TerrainFeature::Water:
						c.b = glm::mix(c.b, b.b, k * pressure);
						break;
					case TerrainFeature::Fords:
						c.r = glm::mix(c.r, b.r, k * pressure);
						break;
				}
				_groundmap->set_pixel(p.x, p.y, c);
			}
		}

	return bounds2_from_center(position, radius + 1);
}


bounds2f SmoothTerrainSurface::Paint(TerrainFeature feature, glm::vec2 position, float radius, float pressure)
{
	glm::vec2 scale = _bounds.size() / glm::vec2(_groundmap->size());
	float abs_pressure = glm::abs(pressure);

	glm::ivec2 center = MapWorldToImage(position);

	float value = pressure > 0 ? 1 : 0;
	float delta = pressure > 0 ? 0.015f : -0.015f;

	for (int x = -10; x <= 10; ++x)
		for (int y = -10; y <= 10; ++y)
		{
			glm::ivec2 p = center + glm::ivec2(x, y);
			float d = glm::distance(position, scale * glm::vec2(p)) / radius;
			float k = 1.0f - d * d;
			if (k > 0)
			{
				glm::vec4 c = _groundmap->get_pixel(p.x, p.y);
				switch (feature)
				{
					case TerrainFeature::Hills:
						c.a = glm::mix(c.a, c.a + delta, k * abs_pressure);
						break;
					case TerrainFeature::Trees:
						c.g = glm::mix(c.g, value, k * abs_pressure);
						break;
					case TerrainFeature::Water:
						c.b = glm::mix(c.b, value, k * abs_pressure);
						break;
					case TerrainFeature::Fords:
						c.r = glm::mix(c.r, value, k * abs_pressure);
						break;
				}
				_groundmap->set_pixel(p.x, p.y, c);
			}
		}

	return bounds2_from_center(position, radius + 1);
}


glm::ivec2 SmoothTerrainSurface::MapWorldToImage(glm::vec2 position) const
{
	glm::vec2 p = (position - _bounds.min) / _bounds.size();
	glm::ivec2 s = _groundmap->size();
	return glm::ivec2((int)(p.x * s.x), (int)(p.y * s.y));
}



#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES // detect objective-c
static NSString* FramebufferStatusString(GLenum status)
{
	switch (status)
	{
		case GL_FRAMEBUFFER_COMPLETE:
			return @"GL_FRAMEBUFFER_COMPLETE";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			return @"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			return @"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: return @"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
#endif
		case GL_FRAMEBUFFER_UNSUPPORTED:
			return @"GL_FRAMEBUFFER_UNSUPPORTED";
		default:
			return [NSString stringWithFormat:@"0x%04x", (unsigned int)status];
	}
}
#endif



void SmoothTerrainSurface::EnableRenderEdges()
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


void SmoothTerrainSurface::UpdateHeights()
{
	int n = _size - 1;

	for (int x = 0; x <= n; x += 2)
		for (int y = 0; y <= n; y += 2)
		{
			int i = x + y * _size;
			_heights[i] = CalculateHeight(x, y);
		}

	for (int x = 1; x < n; x += 2)
		for (int y = 1; y < n; y += 2)
		{
			int i = x + y * _size;
			_heights[i] = CalculateHeight(x, y);
		}

	for (int y = 0; y <= n; y += 2)
		for (int x = 1; x < n; x += 2)
		{
			int i = x + y * _size;
			_heights[i] = 0.5f * (_heights[i - 1] + _heights[i + 1]);
		}

	for (int y = 1; y < n; y += 2)
		for (int x = 0; x <= n; x += 2)
		{
			int i = x + y * _size;
			_heights[i] = 0.5f * (_heights[i - _size] + _heights[i + _size]);
		}
}


float SmoothTerrainSurface::CalculateHeight(int x, int y) const
{
	glm::vec4 color = _groundmap->get_pixel(x, y);
	glm::vec4 color_xn = _groundmap->get_pixel(x - 1, y);
	glm::vec4 color_xp = _groundmap->get_pixel(x + 1, y);
	glm::vec4 color_yn = _groundmap->get_pixel(x, y - 1);
	glm::vec4 color_yp = _groundmap->get_pixel(x, y + 1);

	float alpha = 0.5 * color.a + 0.125 * (color_xn.a + color_xp.a + color_yn.a + color_yp.a);

	float height = 0.5f + 124.5f * alpha;

	float water = color.b;
	height = glm::mix(height, -2.5f, water);

	float fords = color.r;
	height = glm::mix(height, -0.5f, fords);

	return height;
}


void SmoothTerrainSurface::UpdateNormals()
{
	glm::vec2 size = _bounds.size();

	int n = _size - 1;
	float k = n;
	glm::vec2 delta = 2.0f * size / k;
	int index = 0;
	for (int y = 0; y <= n; ++y)
	{
		for (int x = 0; x <= n; ++x)
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


static float nearest_odd(float value)
{
	return 1.0f + 2.0f * (int)glm::round(0.5f * (value - 1.0f));
}


float SmoothTerrainSurface::InterpolateHeight(glm::vec2 position) const
{
	glm::vec2 p = (position - _bounds.min) / _bounds.size();
	float n = _size - 1;
	float x = p.x * n;
	float y = p.y * n;

	// find triangle midpoint coordinates (x1, y1)

	float x1 = nearest_odd(x);
	float y1 = nearest_odd(y);

	// find triangle {(x1, y1), (x2, y2), (x3, y3)} containing (x, y)

	float sx2, sx3, sy2, sy3;
	float dx = x - x1;
	float dy = y - y1;
	if (glm::abs(dx) > glm::abs(dy))
	{
		sx2 = sx3 = dx < 0.0f ? -1.0f : 1.0f;
		sy2 = -1;
		sy3 = 1;
	}
	else
	{
		sx2 = -1;
		sx3 = 1;
		sy2 = sy3 = dy < 0.0f ? -1.0f : 1.0f;
	}

	float x2 = x1 + sx2;
	float x3 = x1 + sx3;
	float y2 = y1 + sy2;
	float y3 = y1 + sy3;

	// get heigts for triangle vertices

	float h1 = GetHeight((int)x1, (int)y1);
	float h2 = GetHeight((int)x2, (int)y2);
	float h3 = GetHeight((int)x3, (int)y3);

	// calculate barycentric coordinates k1, k2, k3
	// note: scale of each k is twice the normal

	float k2 = dx * sx2 + dy * sy2;
	float k3 = dx * sx3 + dy * sy3;
	float k1 = 2.0f - k2 - k3;

	return 0.5f * (k1 * h1 + k2 * h2 + k3 * h3);
}


static bool almost_zero(float value)
{
	static const float epsilon = 10 * std::numeric_limits<float>::epsilon();
	return fabsf(value) < epsilon;
}


const float* SmoothTerrainSurface::InternalIntersect(ray r)
{
	static float result;

	bounds1f height = bounds1f(-2.5f, 250);
	bounds2f bounds(0, 0, _size - 1, _size - 1);
	bounds2f quad(-0.01f, -0.01f, 1.01f, 1.01f);

	const float* d = ::intersect(r, bounds3f(bounds, height));
	if (d == nullptr)
		return nullptr;

	glm::vec3 p = r.point(*d);

	bounds2f bounds_2(0, 0, _size - 2, _size - 2);

	int x = (int)bounds_2.x().clamp(p.x);
	int y = (int)bounds_2.y().clamp(p.y);
	int flipX = r.direction.x < 0 ? 0 : 1;
	int flipY = r.direction.y < 0 ? 0 : 1;
	int dx = r.direction.x < 0 ? -1 : 1;
	int dy = r.direction.y < 0 ? -1 : 1;

	while (height.contains(p.z) && bounds_2.contains(x, y))
	{
		glm::vec3 p00 = glm::vec3(x, y, GetHeight(x, y));
		glm::vec3 p10 = glm::vec3(x + 1, y, GetHeight(x + 1, y));
		glm::vec3 p01 = glm::vec3(x, y + 1, GetHeight(x, y + 1));
		glm::vec3 p11 = glm::vec3(x + 1, y + 1, GetHeight(x + 1, y + 1));

		if ((x & 1) == (y & 1))
		{
			d = ::intersect(r, plane(p00, p10, p11));
			if (d != nullptr)
			{
				glm::vec2 rel = (r.point(*d) - p00).xy();
				if (quad.contains(rel) && rel.x >= rel.y)
				{
					result = *d;
					return &result;
				}
			}

			d = ::intersect(r, plane(p00, p11, p01));
			if (d != nullptr)
			{
				glm::vec2 rel = (r.point(*d) - p00).xy();
				if (quad.contains(rel) && rel.x <= rel.y)
				{
					result = *d;
					return &result;
				}
			}
		}
		else
		{
			d = ::intersect(r, plane(p11, p01, p10));
			if (d != nullptr)
			{
				glm::vec2 rel = (r.point(*d) - p00).xy();
				if (quad.contains(rel) && rel.x >= 1 - rel.y)
				{
					result = *d;
					return &result;
				}
			}

			d = ::intersect(r, plane(p00, p10, p01));
			if (d != nullptr)
			{
				glm::vec2 rel = (r.point(*d) - p00).xy();
				if (quad.contains(rel) && rel.x <= 1 - rel.y)
				{
					result = *d;
					return &result;
				}
			}
		}

		float xDist = almost_zero(r.direction.x) ? std::numeric_limits<float>::max() : (x - p.x + flipX) / r.direction.x;
		float yDist = almost_zero(r.direction.y) ? std::numeric_limits<float>::max() : (y - p.y + flipY) / r.direction.y;

		if (xDist < yDist)
		{
			x += dx;
			p += r.direction * xDist;
		}
		else
		{
			y += dy;
			p += r.direction * yDist;
		}
	}

	return nullptr;
}



void SmoothTerrainSurface::UpdateDepthTextureSize()
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


void SmoothTerrainSurface::InitializeShadow()
{
	glm::vec2 center = _bounds.center();
	float radius1 = _bounds.width() / 2;
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


void SmoothTerrainSurface::InitializeSkirt()
{
	glm::vec2 center = _bounds.center();
	float radius = _bounds.width() / 2;

	_vboSkirt._mode = GL_TRIANGLE_STRIP;
	_vboSkirt._vertices.clear();

	int n = 1024;
	float d = 2 * (float)M_PI / n;
	for (int i = 0; i < n; ++i)
	{
		float a = d * i;
		glm::vec2 p = center + radius * vector2_from_angle(a);
		float h = fmaxf(0, InterpolateHeight(p));

		_vboSkirt._vertices.push_back(skirt_vertex(glm::vec3(p, h + 0.5), h));
		_vboSkirt._vertices.push_back(skirt_vertex(glm::vec3(p, -2.5), h));
	}

	_vboSkirt._vertices.push_back(_vboSkirt._vertices[0]);
	_vboSkirt._vertices.push_back(_vboSkirt._vertices[1]);

	_vboSkirt.update(GL_STATIC_DRAW);
}


void SmoothTerrainSurface::UpdateSplatmap()
{
	glm::ivec2 size = _groundmap->size();

	GLubyte* data = new GLubyte[4 * size.x * size.y];
	if (data != nullptr)
	{
		GLubyte* p = data;
		for (int y = 0; y < size.y; ++y)
			for (int x = 0; x < size.x; ++x)
			{
				float forest = GetForestValue(x, y);
				float block = GetImpassableValue(x, y);
				*p++ = (GLubyte)(255.0f * block);
				*p++ = (GLubyte)(255.0f * forest);
				*p++ = (GLubyte)(255.0f * forest);
				*p++ = (GLubyte)(255.0f * forest);
			}

		glBindTexture(GL_TEXTURE_2D, _splatmap->id);
		CHECK_ERROR_GL();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		CHECK_ERROR_GL();
		glGenerateMipmap(GL_TEXTURE_2D);
		CHECK_ERROR_GL();

		delete[] data;
	}
}


float SmoothTerrainSurface::GetForestValue(int x, int y) const
{
	glm::vec4 c = _groundmap->get_pixel(x, y);
	return c.g;
}


float SmoothTerrainSurface::GetImpassableValue(int x, int y) const
{
	glm::vec4 c = _groundmap->get_pixel(x, y);
	if (c.b >= 0.5f && c.r < 0.5f)
		return 1.0f;

	glm::vec3 n = GetNormal(x, y);

	return bounds1f(0, 1).clamp(0.5f + 8.0f * (0.83f - n.z));
}


void SmoothTerrainSurface::UpdateChanges(bounds2f bounds)
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


void SmoothTerrainSurface::InitializeLines()
{
	glm::vec2 corner = _bounds.min;
	glm::vec2 size = _bounds.size();

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


void SmoothTerrainSurface::BuildTriangles()
{
	glm::vec2 corner = _bounds.p11();
	glm::vec2 size = _bounds.size();

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

			PushTriangle(v00, v20, v11);
			PushTriangle(v20, v22, v11);
			PushTriangle(v22, v02, v11);
			PushTriangle(v02, v00, v11);
		}

	_vboInside.update(GL_STATIC_DRAW);
	_vboBorder.update(GL_STATIC_DRAW);
}


void SmoothTerrainSurface::PushTriangle(const terrain_vertex& v0, const terrain_vertex& v1, const terrain_vertex& v2)
{
	vertexbuffer<terrain_vertex>* s = SelectTerrainVbo(inside_circle(_bounds, v0, v1, v2));
	if (s != nullptr)
	{
		s->_vertices.push_back(v0);
		s->_vertices.push_back(v1);
		s->_vertices.push_back(v2);
	}
}


vertexbuffer<terrain_vertex>* SmoothTerrainSurface::SelectTerrainVbo(int inside)
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
