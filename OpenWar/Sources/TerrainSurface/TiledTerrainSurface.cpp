// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TiledTerrainSurface.h"


TiledTerrainSurface::TiledTerrainSurface(bounds2f bounds, glm::ivec2 size) :
_bounds(bounds),
_size(size),
_tiles(nullptr),
_heightmap(nullptr),
_nextTextureNumber(1)
{
	_tiles = new Tile[size.x * size.y];
	_heightmap = new heightmap(glm::ivec2(size.x + 1, size.y + 1));
}


TiledTerrainSurface::~TiledTerrainSurface()
{
	delete [] _tiles;
	delete _heightmap;
}


float TiledTerrainSurface::GetHeight(glm::vec2 position) const
{
	glm::ivec2 size = _heightmap->size();
	glm::vec2 c = glm::vec2(size.x - 1, size.y - 1) * (position - _bounds.p11()) / _bounds.size();

	return _heightmap->interpolate(c);
}


glm::vec3 TiledTerrainSurface::GetNormal(glm::vec2 position) const
{
	return glm::vec3(0, 0, 1);
}


float const* TiledTerrainSurface::Intersect(ray r)
{
	bounds2f bounds = GetBounds();
	glm::vec3 offset = glm::vec3(bounds.min, 0);
	glm::vec3 scale = glm::vec3(glm::vec2(_heightmap->size().x - 1, _heightmap->size().y - 1) / bounds.size(), 1);

	ray r2 = ray(scale * (r.origin - offset), glm::normalize(scale * r.direction));
	const float* d = _heightmap->intersect(r2);
	if (d == nullptr)
		return nullptr;

	static float result;
	result = glm::length((r2.point(*d) - r2.origin) / scale);
	return &result;
}


bool TiledTerrainSurface::IsWater(glm::vec2 position) const
{
	return false;
}


bool TiledTerrainSurface::IsForest(glm::vec2 position) const
{
	return false;
}


bool TiledTerrainSurface::IsImpassable(glm::vec2 position) const
{
	return false;
}


bool TiledTerrainSurface::ContainsWater(bounds2f bounds) const
{
	return false;
}


void TiledTerrainSurface::SetHeight(int x, int y, float h)
{
	_heightmap->set_height(x, y, h);
}



void TiledTerrainSurface::SetTile(int x, int y, const std::string& texture, int rotate, bool mirror)
{
	NSString* path = [NSString stringWithCString:texture.c_str() encoding:NSASCIIStringEncoding];

	if (_textureNumber.find(texture) == _textureNumber.end())
	{
		::texture* t = new ::texture(image(path));

		glBindTexture(GL_TEXTURE_2D, t->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		_textures[_nextTextureNumber] = t;
		_textureNumber[texture] = _nextTextureNumber;
		++_nextTextureNumber;
	}

	TiledTerrainSurface::Tile* tile = GetTile(x, y);
	tile->texture = _textures[_textureNumber[texture]];
	tile->rotate = rotate;
	tile->mirror = mirror;
}


TiledTerrainSurface::Tile* TiledTerrainSurface::GetTile(int x, int y)
{
	if (0 <= x && x < _size.x && 0 <= y && y < _size.y)
		return _tiles + x + _size.x * y;

	return nullptr;
}
