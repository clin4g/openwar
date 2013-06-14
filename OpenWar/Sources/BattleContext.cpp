// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleContext.h"


BattleContext::BattleContext() :
simulationState(nullptr),
simulationRules(nullptr),
smoothTerrainModel(nullptr),
tiledTerrainModel(nullptr),
battleModel(nullptr)
{
}


BattleContext::~BattleContext()
{
}
