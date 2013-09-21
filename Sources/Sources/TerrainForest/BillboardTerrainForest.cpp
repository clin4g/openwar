// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BillboardTerrainForest.h"


BillboardTerrainForest::BillboardTerrainForest()
{
}


BillboardTerrainForest::~BillboardTerrainForest()
{
}


void BillboardTerrainForest::AddTree(glm::vec2 position)
{
	_trees.push_back(position);
}
