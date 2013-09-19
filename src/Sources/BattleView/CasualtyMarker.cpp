// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "CasualtyMarker.h"
#include "ColorBillboardRenderer.h"
#include "TextureBillboardRenderer.h"
#include "TerrainSurface.h"



CasualtyMarker::CasualtyMarker(BattleModel* battleModel) :
_battleModel(battleModel)
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



void CasualtyMarker::RenderCasualtyColorBillboards(ColorBillboardRenderer* renderer)
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
			renderer->AddBillboard(casualty.position, c, 6.0);
		}
	}
}


void CasualtyMarker::AppendCasualtyBillboards(BillboardModel* billboardModel)
{
	for (const CasualtyMarker::Casualty& casualty : casualties)
	{
		int shape = 0;
		float height = 0;
		//int j = 0, i = 0;
		switch (casualty.platform)
		{
			case UnitPlatformAsh:
				shape = billboardModel->_billboardShapeCasualtyAsh[casualty.seed & 7];
				height = 2.5f;
				//i = 3;
				//j = casualty.seed & 3;
				break;
			case UnitPlatformSam:
				shape = billboardModel->_billboardShapeCasualtySam[casualty.seed & 7];
				height = 2.5f;
				//i = 3;
				//j = 4 + (casualty.seed & 3);
				break;
			case UnitPlatformCav:
			case UnitPlatformGen:
				shape = billboardModel->_billboardShapeCasualtySam[casualty.seed & 15];
				height = 3.0f;
				//i = 4;
				//j = casualty.seed & 7;
				break;
		}

		const float adjust = 0.5 - 2.0 / 64.0; // place texture 2 texels below ground
		glm::vec3 p = _battleModel->terrainSurface->GetPosition(casualty.position.xy(), adjust * height);
		billboardModel->dynamicBillboards.push_back(Billboard(p, 0, height, shape));

	}
}
