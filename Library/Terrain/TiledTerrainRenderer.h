// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TiledTerrainRenderer_H
#define TiledTerrainRenderer_H

#include "TerrainRenderer.h"
#include "texture.h"

class TiledTerrainModel;


class TiledTerrainRenderer : public TerrainRenderer
{
	TiledTerrainModel* _terrainModel;
	std::map<int, texture*> _textures;
	std::map<std::string, int> _textureNumber;
	int _nextTextureNumber;

public:
	TiledTerrainRenderer();
	virtual ~TiledTerrainRenderer();

	TiledTerrainModel* GetTerrainModel() const { return _terrainModel; }

	void SetTile(int x, int y, const std::string& texture, int rotate, bool mirror);

	virtual void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal);
};


#endif
