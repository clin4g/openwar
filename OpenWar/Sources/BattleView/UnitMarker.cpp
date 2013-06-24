// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitMarker.h"
#include "BattleModel.h"
#include "TextureRenderer.h"
#include "geometry.h"


UnitMarker::UnitMarker(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit)
{
}


UnitMarker::~UnitMarker()
{
}


void UnitMarker::_Path(TextureTriangleRenderer* renderer, int mode, float scale, const std::vector<glm::vec2>& path, float t0)
{
	int t1 = 0;
	int t2 = 4;

	switch (mode)
	{
		case 1:
			t1 = 4;
			t2 = 8;
			break;

		case 2:
			t1 = 8;
			t2 = 14;
			break;
	}

	float tx1 = t1 * (1.0f / 16.0f);
	float tx2 = t2 * (1.0f / 16.0f);
	float ty1 = (t0 - glm::length(path[0] - path[1])) / scale / 8.0f;

#if TARGET_OS_IPHONE
	static float adjust = 0;
	if (adjust == 0)
	{
		if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
			adjust = 2;
		else
			adjust = 1;
	}
#else
	static float adjust = 1;
#endif

	for (int i = 1; i < (int)path.size(); ++i)
	{
		glm::vec2 p1 = path[i - 1];
		glm::vec2 p2 = path[i];
		glm::vec2 d = p2 - p1;
		float length = glm::length(d);

		float ty2 = ty1 + length / scale / 8.0f;

		glm::vec2 left;

		if (length > 0.01f)
			left = adjust * scale * rotate(d / length, (float)M_PI_2);

		glm::vec2 p1L = p1 + left;
		glm::vec2 p1R = p1 - left;
		glm::vec2 p2L = p2 + left;
		glm::vec2 p2R = p2 - left;

		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(p1L, 1), glm::vec2(tx1, -ty1));
		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(p2L, 1), glm::vec2(tx1, -ty2));
		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(p2R, 1), glm::vec2(tx2, -ty2));
		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(p2R, 1), glm::vec2(tx2, -ty2));
		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(p1R, 1), glm::vec2(tx2, -ty1));
		renderer->AddVertex(_battleModel->terrainSurface->GetPosition(p1L, 1), glm::vec2(tx1, -ty1));

		ty1 = ty2;
	}
}




void UnitMarker::Path(TextureTriangleRenderer* renderer, int mode, glm::vec2 position, const std::vector<glm::vec2>& path, float t0)
{
	if (path.size() == 0)
		return;

	std::vector<glm::vec2> p;
	p.insert(p.begin(), position);
	p.insert(p.end(), path.begin(), path.end());

	_Path(renderer, mode, 1, p, t0);
}
