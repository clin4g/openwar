// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmoothTerrainSurface.h"
#include "image.h"



SmoothTerrainSurface::SmoothTerrainSurface(bounds2f bounds, image* map) :
_bounds(bounds),
_heightmap(glm::ivec2(128, 128)),
_height(124.5),
_map(map)
{
	glm::vec2 mapsize = glm::vec2(map->size());
	_scaleImageToWorld = bounds.size() / mapsize;

	LoadHeightmapFromImage();
}


SmoothTerrainSurface::~SmoothTerrainSurface()
{
}


float SmoothTerrainSurface::GetHeight(glm::vec2 position) const
{
	glm::ivec2 size = _heightmap.size();
	glm::vec2 c = glm::vec2(size.x - 1, size.y - 1) * (position - _bounds.p11()) / _bounds.size();

	float height = _heightmap.interpolate(c);

	if (_map != nullptr)
	{
		glm::ivec2 coord = MapCoord(position);
		glm::vec4 color = _map->get_pixel(coord.x, coord.y);

		float water = color.b;
		height = glm::mix(height, -2.5f, glm::step(0.5f, water));

		float fords = color.r;
		height = glm::mix(height, -0.5f, glm::step(0.5f, fords));
	}

	return height;
}


glm::vec3 SmoothTerrainSurface::GetNormal(glm::vec2 position) const
{
	float h = GetHeight(position);
	glm::vec3 v1 = glm::vec3(1, 0, GetHeight(position + glm::vec2(1, 0)) - h);
	glm::vec3 v2 = glm::vec3(0, 1, GetHeight(position + glm::vec2(0, 1)) - h);
	return glm::normalize(glm::cross(v1, v2));
}


const float* SmoothTerrainSurface::Intersect(ray r)
{
	bounds2f bounds = GetBounds();
	glm::vec3 offset = glm::vec3(bounds.min, 0);
	glm::vec3 scale = glm::vec3(glm::vec2(_heightmap.size().x - 1, _heightmap.size().y - 1) / bounds.size(), 1);

	ray r2 = ray(scale * (r.origin - offset), glm::normalize(scale * r.direction));
	const float* d = _heightmap.intersect(r2);
	if (d == nullptr)
		return nullptr;

	static float result;
	result = glm::length((r2.point(*d) - r2.origin) / scale);
	return &result;
}


bool SmoothTerrainSurface::IsForest(glm::vec2 position) const
{
	glm::ivec2 coord = MapCoord(position);
	glm::vec4 c = _map->get_pixel(coord.x, coord.y);
	return c.g >= 0.5;
}


bool SmoothTerrainSurface::IsImpassable(glm::vec2 position) const
{
	glm::ivec2 coord = MapCoord(position);
	glm::vec4 c = _map->get_pixel(coord.x, coord.y);
	return c.b >= 0.5 && c.r < 0.5;
}


void SmoothTerrainSurface::LoadHeightmapFromImage()
{
	glm::ivec2 imageSize = _map->size();
	glm::ivec2 heightSize = _heightmap.size();
	glm::vec2 scale = glm::vec2(imageSize) / glm::vec2(heightSize);

	for (int heightX = 0; heightX < heightSize.x; ++heightX)
	{
		int imageX = (int)(heightX * scale.x);
		for (int heightY = 0; heightY < heightSize.y; ++heightY)
		{
			int imageY = (int)(heightY * scale.y);
			glm::vec4 c = _map->get_pixel(imageX, imageY);
			_heightmap.set_height(heightX, heightY, 0.5f + 124.5f * c.a);
		}
	}
}


void SmoothTerrainSurface::SaveHeightmapToImage()
{
	glm::ivec2 imageSize = _map->size();
	glm::ivec2 heightSize = _heightmap.size();
	glm::vec2 scale = glm::vec2(heightSize) / glm::vec2(imageSize);

	for (int imageX = 0; imageX < imageSize.x; ++imageX)
	{
		float heightX = imageX * scale.x;
		for (int imageY = 0; imageY < imageSize.y; ++imageY)
		{
			float heightY = imageY * scale.y;
			glm::vec4 c = _map->get_pixel(imageX, imageY);
			c.a = (glm::round(_heightmap.interpolate(glm::vec2(heightX, heightY))) - 0.5f) / 124.5f;
			_map->set_pixel(imageX, imageY, c);
		}
	}
}


float SmoothTerrainSurface::GetHeight(int x, int y) const
{
	return _heightmap.get_height(x, y);
}


void SmoothTerrainSurface::Extract(glm::vec2 position, image* brush)
{
	glm::ivec2 size = brush->size();
	glm::ivec2 origin = MapCoord(position) - size / 2;

	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
			brush->set_pixel(x, y, _map->get_pixel(origin.x + x, origin.y + y));
}


bounds2f SmoothTerrainSurface::Paint(TerrainFeature feature, glm::vec2 position, image* brush, float pressure)
{
	glm::ivec2 size = brush->size();
	glm::ivec2 center = MapCoord(position);
	glm::ivec2 origin = center - size / 2;
	float radius = size.x / 2.0f;

	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
		{
			glm::ivec2 p = origin + glm::ivec2(x, y);
			float d = glm::distance(position, _scaleImageToWorld * glm::vec2(p)) /  radius;
			float k = 1.0f - d * d;
			if (k > 0)
			{
				glm::vec4 b = brush->get_pixel(x, y);
				glm::vec4 c = _map->get_pixel(p.x, p.y);
				switch (feature)
				{
					case TerrainFeature::Hills: c.a = glm::mix(c.a, b.a, k * pressure); break;
					case TerrainFeature::Trees: c.g = glm::mix(c.g, b.g, k * pressure); break;
					case TerrainFeature::Water: c.b = glm::mix(c.b, b.b, k * pressure); break;
					case TerrainFeature::Fords: c.r = glm::mix(c.r, b.r, k * pressure); break;
				}
				_map->set_pixel(p.x, p.y, c);
			}
		}

	if (feature == TerrainFeature::Hills)
		LoadHeightmapFromImage();

	return bounds2_from_center(position, radius + 1);
}


bounds2f SmoothTerrainSurface::Paint(TerrainFeature feature, glm::vec2 position, float radius, float pressure)
{
	float abs_pressure = glm::abs(pressure);

	glm::ivec2 center = MapCoord(position);

	float value = pressure > 0 ? 1 : 0;
	float delta = pressure > 0 ? 0.015f : -0.015f;

	for (int x = -10; x <= 10; ++x)
		for (int y = -10; y <= 10; ++y)
		{
			glm::ivec2 p = center + glm::ivec2(x, y);
			float d = glm::distance(position, _scaleImageToWorld * glm::vec2(p)) /  radius;
			float k = 1.0f - d * d;
			if (k > 0)
			{
				glm::vec4 c = _map->get_pixel(p.x, p.y);
				switch (feature)
				{
					case TerrainFeature::Hills: c.a = glm::mix(c.a, c.a + delta, k * abs_pressure); break;
					case TerrainFeature::Trees: c.g = glm::mix(c.g, value, k * abs_pressure); break;
					case TerrainFeature::Water: c.b = glm::mix(c.b, value, k * abs_pressure); break;
					case TerrainFeature::Fords: c.r = glm::mix(c.r, value, k * abs_pressure); break;
				}
				_map->set_pixel(p.x, p.y, c);
			}
		}

	if (feature == TerrainFeature::Hills)
		LoadHeightmapFromImage();

	return bounds2_from_center(position, radius + 1);
}



glm::ivec2 SmoothTerrainSurface::MapCoord(glm::vec2 position) const
{
	glm::vec2 p = (position - _bounds.min) / _bounds.size();
	glm::ivec2 s = _map->size();
	return glm::ivec2((int)(p.x * s.x), (int)(p.y * s.y));
}

