// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "CasualtyMarker.h"
#include "BattleRendering.h"



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



void CasualtyMarker::RenderCasualtyColorBillboards(BattleRendering* rendering)
{
	if (casualties.empty())
		return;

	glm::vec4 c1 = glm::vec4(1, 1, 1, 0.8);
	glm::vec4 cr = glm::vec4(1, 0, 0, 0);
	glm::vec4 cb = glm::vec4(0, 0, 1, 0);

	for (const CasualtyMarker::Casualty& casualty : casualties)
	{
		if (casualty.time <= 1)
		{
			glm::vec4 c = glm::mix(c1, casualty.player == Player1 ? cb : cr, casualty.time);
			rendering->_vboColorBillboards._vertices.push_back(BattleRendering::color_billboard_vertex(casualty.position, c, 6.0));
		}
	}
}
