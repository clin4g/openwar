// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TiledTerrainModel.h"


TiledTerrainModel::TiledTerrainModel(bounds2f bounds, glm::ivec2 size) :
_bounds(bounds),
_size(size),
_tiles(nullptr),
_heightmap(nullptr)
{
	_tiles = new Tile[size.x * size.y];
	_heightmap = new heightmap(glm::ivec2(size.x + 1, size.y + 1));
}


TiledTerrainModel::~TiledTerrainModel()
{
	delete [] _tiles;
	delete _heightmap;
}


void TiledTerrainModel::Resize(glm::ivec2 size)
{
	_size = size;

	delete [] _tiles;
	_tiles = new Tile[size.x * size.y];

	delete _heightmap;
	_heightmap = new heightmap(glm::ivec2(size.x + 1, size.y + 1));
}


void TiledTerrainModel::SetHeight(int x, int y, float h)
{
	_heightmap->set_height(x, y, h);
}


float TiledTerrainModel::GetHeight(glm::vec2 position) const
{
	return _heightmap->interpolate(position);
}



TiledTerrainModel::Tile* TiledTerrainModel::GetTile(int x, int y)
{
	if (0 <= x && x < _size.x && 0 <= y && y < _size.y)
		return _tiles + x + _size.x * y;
	return nullptr;
}
