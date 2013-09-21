// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TerrainWater_H
#define TerrainWater_H

#include "../../Library/Algebra/bounds.h"


class TerrainWater
{
public:
	TerrainWater();
	virtual ~TerrainWater();

	virtual bool IsWater(glm::vec2 position) const = 0;
	virtual bool ContainsWater(bounds2f bounds) const = 0;
};



#endif
