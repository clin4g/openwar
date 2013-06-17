// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TerrainFeatureModel.h"


TerrainFeatureModel::TerrainFeatureModel()
{
}


TerrainFeatureModel::~TerrainFeatureModel()
{
}


void TerrainFeatureModel::AddTree(glm::vec2 position)
{
	_trees.push_back(position);
}
