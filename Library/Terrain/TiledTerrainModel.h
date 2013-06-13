// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TiledTerrainModel_H
#define TiledTerrainModel_H

#include "bounds.h"
#include "heightmap.h"


class TiledTerrainModel
{
public:
	struct Tile
	{
		int texture;
		int rotate; // counterclockwise
		bool mirror;

		Tile() : texture(0), rotate(0), mirror(false) { }
	};

private:
	bounds2f _bounds;
	glm::ivec2 _size;
	Tile* _tiles;
	heightmap* _heightmap;

public:
	TiledTerrainModel(bounds2f bounds, glm::ivec2 size);
	~TiledTerrainModel();

	void Resize(glm::ivec2 size);

	void SetHeight(int x, int y, float h);

	float GetHeight(glm::vec2 position) const;

	bounds2f GetBounds() const { return _bounds; }
	glm::ivec2 GetSize() const { return _size; }

	Tile* GetTile(int x, int y);
};


#endif
