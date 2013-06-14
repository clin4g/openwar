// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLECONTEXT_H
#define BATTLECONTEXT_H

class BattleModel;
class BattleView;
class SimulationRules;
class SimulationState;
class SmoothTerrainModel;
class SmoothTerrainRenderer;
class TiledTerrainModel;
class TiledTerrainRenderer;


class BattleContext
{
public:
	BattleContext();
	~BattleContext();

	SimulationState* simulationState;
	SimulationRules* simulationRules;

	SmoothTerrainModel* smoothTerrainModel;
	SmoothTerrainRenderer* smoothTerrainRendering;

	TiledTerrainModel* tiledTerrainModel;
	TiledTerrainRenderer* tiledTerrainRenderer;

	BattleModel* battleModel;
	BattleView* battleView;
};



#endif
