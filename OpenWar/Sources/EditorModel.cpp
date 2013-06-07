/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "EditorModel.h"
#include "BattleModel.h"
#include "BattleView.h"



EditorModel::EditorModel(BattleView* battleView, SmoothTerrainRendering* terrainRendering) :
_battleView(battleView),
_terrainRendering(terrainRendering),
editorMode(EditorMode::Hand),
editorFeature(EditorFeature::Hills)
{
}


void EditorModel::ToolBegan(glm::vec2 position)
{
	switch (editorFeature)
	{
		case EditorFeature::Hills:
			EditHills(position, editorMode == EditorMode::Paint);
			break;

		case EditorFeature::Water:
			EditWater(position, editorMode == EditorMode::Paint);
			break;

		case EditorFeature::Trees:
			EditTrees(position, editorMode == EditorMode::Paint);
			break;
	}
}


void EditorModel::ToolMoved(glm::vec2 position)
{
	switch (editorFeature)
	{
		case EditorFeature::Hills:
			EditHills(position, editorMode == EditorMode::Paint);
			break;

		case EditorFeature::Water:
			EditWater(position, editorMode == EditorMode::Paint);
			break;

		case EditorFeature::Trees:
			EditTrees(position, editorMode == EditorMode::Paint);
			break;
	}
}


void EditorModel::ToolEnded(glm::vec2 position)
{
	switch (editorFeature)
	{
		case EditorFeature::Hills:
			EditHills(position, editorMode == EditorMode::Paint);
			break;

		case EditorFeature::Water:
			EditWater(position, editorMode == EditorMode::Paint);
			break;

		case EditorFeature::Trees:
			EditTrees(position, editorMode == EditorMode::Paint);
			break;
	}
}


void EditorModel::EditHills(glm::vec2 position, bool value)
{
	float delta = value ? 0.1f : -0.1f;
	int x0 = (int)(_terrainRendering->_terrainModel->GetSize().m * position.x / 1024);
	int y0 = (int)(_terrainRendering->_terrainModel->GetSize().n * position.y / 1024);

	for (int x = -5; x <= 5; ++x)
		for (int y = -5; y <= 5; ++y)
		{
			float h = _terrainRendering->_terrainModel->GetHeight(x0 + x, y0 + y);
			_terrainRendering->_terrainModel->SetHeight(x0 + x, y0 + y, fmaxf(0.1f, h + delta));
		}

	_terrainRendering->UpdateHeights(bounds2_from_center(position, 25));
	_battleView->UpdateTerrainTrees(bounds2_from_center(position, 25));
}


void EditorModel::EditWater(glm::vec2 position, bool value)
{
	int x0 = (int)(512 * position.x / 1024);
	int y0 = (int)(512 * position.y / 1024);

	image* map = _terrainRendering->_terrainModel->_map;

	for (int x = -5; x <= 5; ++x)
		for (int y = -5; y <= 5; ++y)
		{
			glm::vec4 c = map->get_pixel(x, y);
			c.b = value ? 1 : 0;
			map->set_pixel(x0 + x, y0 - y, c);
		}

	_terrainRendering->UpdateHeights(bounds2_from_center(position, 25));
	_battleView->UpdateTerrainTrees(bounds2_from_center(position, 25));
}


void EditorModel::EditTrees(glm::vec2 position, bool value)
{
	int x0 = (int)(512 * position.x / 1024);
	int y0 = (int)(512 * position.y / 1024);

	image* map = _terrainRendering->_terrainModel->_map;

	for (int x = -5; x <= 5; ++x)
		for (int y = -5; y <= 5; ++y)
		{
			glm::vec4 c = map->get_pixel(x, y);
			c.g = value ? 1 : 0;
			map->set_pixel(x0 + x, y0 - y, c);
		}

	_terrainRendering->_map->load(*map);
	_battleView->UpdateTerrainTrees(bounds2_from_center(position, 25));
}
