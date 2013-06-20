// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TerrainFeatureModelBillboard.h"


TerrainFeatureModelBillboard::TerrainFeatureModelBillboard()
{
}


TerrainFeatureModelBillboard::~TerrainFeatureModelBillboard()
{
}


void TerrainFeatureModelBillboard::AddTree(glm::vec2 position)
{
	_trees.push_back(position);
}
