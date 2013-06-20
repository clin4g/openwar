// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLECONTEXT_H
#define BATTLECONTEXT_H

class BattleModel;
class BattleScript;
class BillboardModel;
class SimulationRules;
class BattleModel;
class TerrainSurface;
class TerrainForest;
class BillboardTerrainForest;
class TerrainFeatureModelMesh;


class BattleContext
{
public:
	BattleContext();
	~BattleContext();

	BattleScript* battleScript;
	BattleModel* battleModel;

	SimulationRules* simulationRules;

	BillboardTerrainForest* terrainForest;

	BillboardModel* billboardTextureAtlas;
};


#endif
