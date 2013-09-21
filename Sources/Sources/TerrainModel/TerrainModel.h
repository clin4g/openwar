// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TerrainModel_H
#define TerrainModel_H

class BillboardTerrainForest;
class SmoothTerrainSky;
class SmoothTerrainWater;
class TerrainSurface;


class TerrainModel
{
public:
	TerrainSurface* terrainSurface;
	BillboardTerrainForest* terrainForest;
	SmoothTerrainWater* terrainWater;
	SmoothTerrainSky* terrainSky;

public:
	TerrainModel();
	virtual ~TerrainModel();
};


#endif
