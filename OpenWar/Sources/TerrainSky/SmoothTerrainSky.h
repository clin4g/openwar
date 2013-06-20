// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainSky_H
#define SmoothTerrainSky_H

#include "TerrainSky.h"
#include "renderer.h"


class SmoothTerrainSky : public TerrainSky
{
public:
	SmoothTerrainSky();
	virtual ~SmoothTerrainSky();

	void Render(renderers* _renderers, float cameraDirectionZ, bool flip);
};



#endif
