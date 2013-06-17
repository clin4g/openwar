// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TERRAINFEATUREMODEL_H
#define TERRAINFEATUREMODEL_H


class TerrainFeatureModel
{
public:
	TerrainFeatureModel();
	~TerrainFeatureModel();

	std::vector<glm::vec2> _trees;
	void AddTree(glm::vec2 position);
};


#endif
