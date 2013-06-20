// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TERRAINFEATUREMODELBILLBOARD_H
#define TERRAINFEATUREMODELBILLBOARD_H

#include "TerrainForest.h"


class BillboardTerrainForest : public TerrainForest
{
public:
	BillboardTerrainForest();
	virtual ~BillboardTerrainForest();

	std::vector<glm::vec2> _trees;
	void AddTree(glm::vec2 position);
};


#endif
