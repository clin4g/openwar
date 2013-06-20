// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleContext.h"


BattleContext::BattleContext() :
battleScript(nullptr),
simulationState(nullptr),
simulationRules(nullptr),
terrainSurfaceModel(nullptr),
terrainFeatureModelBillboard(nullptr),
billboardTextureAtlas(nullptr),
battleModel(nullptr)
{
}



BattleContext::~BattleContext()
{
}
