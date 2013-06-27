// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitMarker.h"
#include "BattleModel.h"
#include "TextureRenderer.h"
#include "GradientRenderer.h"
#include "bspline.h"



/***/



/***/




UnitMarker::UnitMarker(BattleModel* battleModel, Unit* unit) :
_battleModel(battleModel),
_unit(unit)
{
}


UnitMarker::~UnitMarker()
{
}




void UnitMarker::RenderPath(GradientLineRenderer* renderer, const std::vector<glm::vec2>& path)
{
	TerrainSurface* terrainSurface = _battleModel->terrainSurface;

	float c = 0;
	for (std::vector<glm::vec2>::const_iterator i = path.begin() + 1; i < path.end(); ++i)
	{
		glm::vec3 p1 = terrainSurface->GetPosition(*(i - 1), 1);
		glm::vec3 p2 = terrainSurface->GetPosition(*i, 1);

		renderer->AddLine(p1, p2, glm::vec4(c, c, c, 1), glm::vec4(c, c, c, 1));
		//c = 1 - c;
	}
}


/*void UnitMarker::_PathLines2(GradientLineRenderer* renderer, const std::vector<glm::vec2>& path, const std::vector<glm::vec2>& original)
{
	TerrainSurface* terrainSurface = _battleModel->terrainSurface;

	float c = 0;
	for (int i = 1; i < (int)path.size(); ++i)
	{
		glm::vec2 p1 = path[i - 1];
		glm::vec2 p2 = path[i];

		glm::vec2 q1 = original[i - 1];
		glm::vec2 q2 = original[i];

		glm::vec4 color = glm::vec4(c, c, c, 1);

		if (glm::dot(p2 - p1, q2 - q1) < 0)
			color = glm::vec4(1, 0, 0, 1);

		renderer->AddLine(terrainSurface->GetPosition(p1, 1), terrainSurface->GetPosition(p2, 1), color, color);
		//c = 1 - c;
	}
}*/



static void InsertArrow1(std::vector<glm::vec2>& path, std::vector<glm::vec2>::iterator i)
{
	glm::vec2 left = *(i - 1);
	glm::vec2 right = *i;
	glm::vec2 center = 0.5f * (left + right);

	float width = glm::length(right - left);
	float w = 0.333f * width;
	float h = 1.333f * width;

	glm::vec2 dir = glm::normalize(left - right);
	glm::vec2 front = glm::vec2(dir.y, -dir.x);

	i = path.insert(i, left + w * dir) + 1;
	i = path.insert(i, center + h * front) + 1;
	path.insert(i, right - w * dir);
}


static void InsertArrow2(std::vector<glm::vec2>& path, std::vector<glm::vec2>::iterator i)
{
	glm::vec2 left1 = *(i - 1);
	glm::vec2 right1 = *i;
	glm::vec2 center1 = 0.5f * (left1 + right1);

	float width = glm::length(right1 - left1);
	float d = 0.500f * width;
	float w = 0.333f * width;
	float h = 1.333f * width;

	glm::vec2 dir = glm::normalize(left1 - right1);
	glm::vec2 front = glm::vec2(dir.y, -dir.x);
	glm::vec2 left2 = left1 + d * front;
	glm::vec2 right2 = right1 + d * front;
	glm::vec2 center2 = center1 + d * front;

	i = path.insert(i, left1 + w * dir) + 1;
	i = path.insert(i, left2) + 1;
	i = path.insert(i, left2 + w * dir) + 1;
	i = path.insert(i, center2 + h * front) + 1;
	i = path.insert(i, right2 - w * dir) + 1;
	i = path.insert(i, right2) + 1;
	path.insert(i, right1 - w * dir);
}


static void InsertStar(std::vector<glm::vec2>& path, std::vector<glm::vec2>::iterator i)
{
	glm::vec2 left = *(i - 1);
	glm::vec2 right = *i;

	float width = glm::length(right - left);

	glm::vec2 dir = glm::normalize(left - right);
	glm::vec2 front = glm::vec2(dir.y, -dir.x);
	glm::vec2 center = 0.5f * (left + right) + 0.500f * width * front;

	float r1 = 0.9f * glm::length(right - center);
	float r2 = 1.8f * r1;

	float a1 = angle(right - center);
	float a2 = angle(left - center);
	if (a2 < a1)
		a2 += 2 * glm::pi<float>();

	int n = 11;
	float d = (a2 - a1) / (n - 1);
	for (int j = 1; j < n - 1; ++j)
	{
		float r = (j & 1) != 0 ? r2 : r1;
		i = path.insert(i, center + r * vector2_from_angle(a2 - d * j)) + 1;
	}
}


void UnitMarker::Path(GradientLineRenderer* renderer, int mode, const std::vector<glm::vec2>& path)
{
	if (path.size() == 0)
		return;

	std::vector<glm::vec2> path2 = path;
	bspline_join(path2, 0.1f);
	bspline_split(path2, 0.1f);

	std::vector<std::pair<glm::vec2, glm::vec2>> strip = spline_line_strip(path2);

	std::vector<glm::vec2> stripp;
	for (std::pair<glm::vec2, glm::vec2> i : strip)
		stripp.push_back(i.first);

	std::vector<glm::vec2> pathL = bspline_offset(strip, 7);
	std::vector<glm::vec2> pathR = bspline_offset(strip, -7);

	if (pathL.size() > 1)
	{
		std::vector<glm::vec2> path3 = pathL;
		path3.insert(path3.end(), pathR.rbegin(), pathR.rend());

		switch (mode)
		{
			case 2:
				InsertStar(path3, path3.begin() + pathL.size());
				break;
			case 1:
				InsertArrow2(path3, path3.begin() + pathL.size());
				break;
			default:
				InsertArrow1(path3, path3.begin() + pathL.size());
				break;
		}

		RenderPath(renderer, path3);
	}
}
