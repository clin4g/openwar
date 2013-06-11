// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmoothTerrainModel.h"
#include "image.h"



SmoothTerrainModel::SmoothTerrainModel(bounds2f bounds, image* map) :
_bounds(bounds),
_heightmap(glm::ivec2(128, 128)),
_height(124.5),
_map(map)
{
	glm::vec2 mapsize = glm::vec2(map->size());
	_scaleImageToWorld = bounds.size() / mapsize;
	_scaleWorldToImage = mapsize / bounds.size();

	LoadHeightmapFromImage();
}


SmoothTerrainModel::~SmoothTerrainModel()
{
}


void SmoothTerrainModel::LoadHeightmapFromImage()
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


void SmoothTerrainModel::SaveHeightmapToImage()
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


float SmoothTerrainModel::GetHeight(int x, int y) const
{
	return _heightmap.get_height(x, y);
}


float SmoothTerrainModel::GetHeight(glm::vec2 position) const
{
	glm::ivec2 size = _heightmap.size();
	glm::vec2 c = glm::vec2(size.x - 1, size.y - 1) * (position - _bounds.p11()) / _bounds.size();

	float height = _heightmap.interpolate(c);

	if (_map != nullptr)
	{
		glm::vec4 color = _map->get_pixel((int)(position.x * 512.0 / 1024.0), (int)(position.y * 512.0 / 1024.0));

		float water = color.b;
		height = glm::mix(height, -2.5f, glm::step(0.5f, water));

		float fords = color.r;
		height = glm::mix(height, -0.5f, glm::step(0.5f, fords));
	}

	return height;
}


glm::vec3 SmoothTerrainModel::GetNormal(glm::vec2 position) const
{
	float h = GetHeight(position);
	glm::vec3 v1 = glm::vec3(1, 0, GetHeight(position + glm::vec2(1, 0)) - h);
	glm::vec3 v2 = glm::vec3(0, 1, GetHeight(position + glm::vec2(0, 1)) - h);
	return glm::normalize(glm::cross(v1, v2));
}


bool SmoothTerrainModel::ContainsWater(bounds2f bounds) const
{
	glm::ivec2 size(512, 512);
	glm::vec2 min = glm::vec2(size.x - 1, size.y - 1) * (bounds.min - _bounds.min) / _bounds.size();
	glm::vec2 max = glm::vec2(size.x - 1, size.y - 1) * (bounds.max - _bounds.min) / _bounds.size();
	int xmin = (int)floorf(min.x);
	int ymin = (int)floorf(min.y);
	int xmax = (int)ceilf(max.x);
	int ymax = (int)ceilf(max.y);

	if (_map != nullptr)
	{
		for (int x = xmin; x <= xmax; ++x)
			for (int y = ymin; y <= ymax; ++y)
			{
				glm::vec4 c = _map->get_pixel(x, y);
				if (c.b >= 0.5 || c.r >= 0.5)
					return true;
			}
	}

	return false;
}


const float* SmoothTerrainModel::Intersect(ray r)
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


bounds2f SmoothTerrainModel::EditHills(glm::vec2 position, float radius, float pressure)
{
	float delta = pressure > 0 ? 0.5f : -0.5f;
	float abs_pressure = glm::abs(pressure);

	glm::ivec2 p0 = glm::ivec2(128.0f / 1024.0f * position);

	for (int x = -5; x <= 5; ++x)
		for (int y = -5; y <= 5; ++y)
		{
			glm::ivec2 pi = p0 + glm::ivec2(x, y);
			float k = 1.0f - glm::distance(position, 1024.0f / 128.0f * glm::vec2(pi)) / radius;
			if (k > 0)
			{
				float h = GetHeight(pi.x, pi.y);
				h = glm::mix(h, h + delta, k * abs_pressure);
				_heightmap.set_height(pi.x, pi.y, fmaxf(0.1f, h));
			}
		}

	return bounds2_from_center(position, radius + 1);
}


bounds2f SmoothTerrainModel::EditWater(glm::vec2 position, float radius, float pressure)
{
	glm::ivec2 p0 = glm::ivec2(_scaleWorldToImage * position);

	float value = pressure > 0 ? 1 : 0;
	float abs_pressure = glm::abs(pressure);

	for (int x = -5; x <= 5; ++x)
		for (int y = -5; y <= 5; ++y)
		{
			glm::ivec2 pi = p0 + glm::ivec2(x, y);
			float k = 1.0f - glm::distance(position, _scaleImageToWorld * glm::vec2(pi)) / radius;
			if (k > 0)
			{
				glm::vec4 c = _map->get_pixel(pi.x, pi.y);
				c.b = glm::mix(c.b, value, k * abs_pressure);
				_map->set_pixel(pi.x, pi.y, c);
			}
		}

	return bounds2_from_center(position, radius + 1);
}


bounds2f SmoothTerrainModel::EditTrees(glm::vec2 position, float radius, float pressure)
{
	glm::ivec2 p0 = glm::ivec2(_scaleWorldToImage * position);

	float value = pressure > 0 ? 1 : 0;
	float abs_pressure = glm::abs(pressure);

	for (int x = -5; x <= 5; ++x)
		for (int y = -5; y <= 5; ++y)
		{
			glm::ivec2 pi = p0 + glm::ivec2(x, y);
			float k = 1.0f - glm::distance(position, _scaleImageToWorld * glm::vec2(pi)) / radius;
			if (k > 0)
			{
				glm::vec4 c = _map->get_pixel(pi.x, pi.y);
				c.g = glm::mix(c.g, value, k * abs_pressure);
				_map->set_pixel(pi.x, pi.y, c);
			}
		}

	return bounds2_from_center(position, radius + 1);
}
