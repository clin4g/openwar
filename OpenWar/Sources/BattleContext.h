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
class TerrainSurfaceModel;
class TerrainFeatureModel;
class TerrainFeatureModelBillboard;
class TerrainFeatureModelMesh;


class BattleContext
{
public:
	BattleContext();
	~BattleContext();

	BattleScript* battleScript;
	BattleModel* battleModel;

	SimulationRules* simulationRules;

	TerrainFeatureModelBillboard* terrainFeatureModelBillboard;

	BillboardModel* billboardTextureAtlas;
};


#endif
