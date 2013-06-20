// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLECONTEXT_H
#define BATTLECONTEXT_H

class BattleModel;
class BattleScript;
class BillboardModel;
class SimulationRules;
class SimulationState;
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

	SimulationState* simulationState;
	SimulationRules* simulationRules;

	TerrainSurfaceModel* terrainSurfaceModel;

	TerrainFeatureModelBillboard* terrainFeatureModelBillboard;

	BillboardModel* billboardTextureAtlas;
};


#endif
