// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TerrainModel_H
#define TerrainModel_H


#include "TerrainSurface.h"
class BillboardTerrainForest;


class TerrainModel
{
public:
	TerrainSurface* terrainSurfaceModel;
	BillboardTerrainForest* terrainForest;

public:
	TerrainModel();
	virtual ~TerrainModel();
};


#endif
