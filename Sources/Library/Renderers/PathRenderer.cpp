// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <algorithm>
#include <glm/gtc/constants.hpp>

#include "PathRenderer.h"
#include "GradientRenderer.h"
#include "TextureRenderer.h"
#include "GradientRenderer.h"
#include "../Algorithms/bspline.h"
#include "../Algebra/geometry.h"



PathRenderer::PathRenderer(std::function<glm::vec3(glm::vec2)> getPosition) :
_getPosition(getPosition),
_color(0, 0, 0, 0.15f),
_offset(7)
{
}


PathRenderer::~PathRenderer()
{
}






inline glm::vec2 rotate90(glm::vec2 v)
{
	return glm::vec2(-v.y, v.x);
}

static float gap_radians(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
	return diff_radians(angle(p2 - p1), angle(p3 - p2));
}



static glm::vec2 safe_normalize(glm::vec2 v)
{
	if (v.x * v.x + v.y * v.y < 0.01) return glm::vec2(1, 0);
	return glm::normalize(v);
}


void PathRenderer::RenderPath(GradientTriangleRenderer* renderer, const std::vector<glm::vec2>& path)
{
	if (path.size() < 2)
		return;

	//TerrainSurface* terrainSurface = _battleModel->terrainSurface;

	float width = 2.5f;

	glm::vec4 cleft = _color;
	glm::vec4 cright = glm::vec4(_color.r, _color.g, _color.b, 0);


	glm::vec2 lastL = path[0];
	glm::vec2 currL = path[1];
	glm::vec2 nextL;
	glm::vec2 dir = safe_normalize(currL - lastL);
	glm::vec2 lastR = lastL - width * rotate90(dir);
	glm::vec2 currR;

	for (std::vector<glm::vec2>::const_iterator i = path.begin() + 1; i < path.end() - 1; ++i)
	{
		currL = *i;
		nextL = *(i + 1);
		dir = safe_normalize(currL - lastL);
		float gap = gap_radians(lastL, currL, nextL) / 2;
		currR = currL - width * vector2_from_angle(angle(dir) + glm::half_pi<float>() - gap);

		glm::vec3 p1 = _getPosition(lastL);
		glm::vec3 p2 = _getPosition(currL);
		glm::vec3 p3 = _getPosition(currR);
		glm::vec3 p4 = _getPosition(lastR);

		renderer->AddVertex(p1, cleft);
		renderer->AddVertex(p2, cleft);
		renderer->AddVertex(p3, cright);

		renderer->AddVertex(p3, cright);
		renderer->AddVertex(p4, cright);
		renderer->AddVertex(p1, cleft);

		lastL = currL;
		lastR = currR;
	}

	currL = path.back();
	dir = safe_normalize(currL - lastL);
	currR = currL - width * rotate90(dir);

	glm::vec3 p1 = _getPosition(lastL);
	glm::vec3 p2 = _getPosition(currL);
	glm::vec3 p3 = _getPosition(currR);
	glm::vec3 p4 = _getPosition(lastR);

	renderer->AddVertex(p1, cleft);
	renderer->AddVertex(p2, cleft);
	renderer->AddVertex(p3, cright);

	renderer->AddVertex(p3, cright);
	renderer->AddVertex(p4, cright);
	renderer->AddVertex(p1, cleft);
}



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


static void bspline_split_segments(std::vector<std::vector<glm::vec2>>& segments, const std::vector<std::pair<glm::vec2, glm::vec2>>& original, const std::vector<glm::vec2>& offset, bool append)
{
	if (!append)
		segments.push_back(std::vector<glm::vec2>());

	int n = (int)original.size();
	for (int i = 1; i < n; ++i)
	{
		glm::vec2 p1 = offset[i - 1];
		glm::vec2 p2 = offset[i];

		glm::vec2 q1 = original[i - 1].first;
		glm::vec2 q2 = original[i].first;

		if (glm::dot(p2 - p1, q2 - q1) < 0)
		{
			if (!segments.back().empty())
				segments.push_back(std::vector<glm::vec2>());
		}
		else
		{
			segments.back().push_back(p1);
		}
	}

	if (!segments.empty())
	{
		if (segments.back().empty())
			segments.pop_back();
		else
			segments.back().push_back(offset.back());
	}
}


void PathRenderer::Path(GradientTriangleRenderer* renderer, const std::vector<glm::vec2>& path, int mode)
{
	if (path.size() == 0)
		return;


	/*for (glm::vec2 p : path)
	{
		glm::vec4 c(0, 0, 0, 0.75f);
		float d = 0.4f;
		glm::vec3 p1 = _battleModel->terrainSurface->GetPosition(glm::vec2(p.x - d, p.y - d), 1);
		glm::vec3 p2 = _battleModel->terrainSurface->GetPosition(glm::vec2(p.x - d, p.y + d), 1);
		glm::vec3 p3 = _battleModel->terrainSurface->GetPosition(glm::vec2(p.x + d, p.y + d), 1);
		glm::vec3 p4 = _battleModel->terrainSurface->GetPosition(glm::vec2(p.x + d, p.y - d), 1);

		renderer->AddVertex(p1, c);
		renderer->AddVertex(p2, c);
		renderer->AddVertex(p3, c);

		renderer->AddVertex(p3, c);
		renderer->AddVertex(p4, c);
		renderer->AddVertex(p1, c);
	}*/


	std::vector<glm::vec2> control_points = path;
	bspline_join(control_points, 0.1f);
	bspline_split(control_points, 0.1f);

	std::vector<std::vector<glm::vec2>> segments;

	std::vector<std::pair<glm::vec2, glm::vec2>> strip = spline_line_strip(control_points);
	std::vector<glm::vec2> pathL = bspline_offset(strip, _offset);
	bspline_split_segments(segments, strip, pathL, false);

	if (segments.empty())
		return;

	int segmentIndex = (int)segments.size() - 1;
	int vertexIndex = (int)segments.back().size();

	std::vector<glm::vec2> pathR = bspline_offset(strip, -_offset);
	std::reverse(strip.begin(), strip.end());
	std::reverse(pathR.begin(), pathR.end());
	bspline_split_segments(segments, strip, pathR, true);

	std::vector<glm::vec2>& head = segments[segmentIndex];
	switch (mode)
	{
		case 2:
			InsertStar(head, head.begin() + vertexIndex);
			break;
		case 1:
			InsertArrow2(head, head.begin() + vertexIndex);
			break;
		default:
			InsertArrow1(head, head.begin() + vertexIndex);
			break;
	}

	//TerrainSurface* terrainSurface = _battleModel->terrainSurface;
	//std::function<glm::vec3(glm::vec2)> getPosition = [terrainSurface](glm::vec2 p) { return terrainSurface->GetPosition(p, 1); };
	for (const std::vector<glm::vec2>& segment : segments)
	{
		RenderPath(renderer, segment);
	}
}
