// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLECONTEXT_H
#define BATTLECONTEXT_H

class BattleModel;
class BillboardTextureAtlas;
class SimulationRules;
class SimulationState;
class TerrainSurfaceModel;
class TerrainSurfaceModelSmooth;
class TerrainSurfaceModelTiled;
class TerrainFeatureModel;
class TerrainFeatureModelBillboard;
class TerrainFeatureModelMesh;


class BattleContext
{
public:
	BattleContext();
	~BattleContext();

	SimulationState* simulationState;
	SimulationRules* simulationRules;

	TerrainSurfaceModelSmooth* terrainSurfaceModelSmooth;
	TerrainSurfaceModelTiled* terrainSurfaceModelTiled;

	TerrainFeatureModelBillboard* terrainFeatureModelBillboard;
	TerrainFeatureModelMesh* terrainFeatureModelMesh;

	BillboardTextureAtlas* billboardTextureAtlas;
	BattleModel* battleModel;

	TerrainSurfaceModel* GetTerrainSurfaceModel() const
	{
		return (TerrainSurfaceModel*)terrainSurfaceModelSmooth ?: (TerrainSurfaceModel*)terrainSurfaceModelTiled;
	}
};


#endif
