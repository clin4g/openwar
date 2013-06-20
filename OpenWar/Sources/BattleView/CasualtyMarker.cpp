// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "CasualtyMarker.h"



CasualtyMarker::CasualtyMarker()
{
}


CasualtyMarker::~CasualtyMarker()
{
}


void CasualtyMarker::AddCasualty(glm::vec3 position, Player player, UnitPlatform platform)
{
	casualties.push_back(Casualty(position, player, platform));
}


bool CasualtyMarker::Animate(float seconds)
{
	for (Casualty& casualty : casualties)
		casualty.time += seconds;

	return true;
}
