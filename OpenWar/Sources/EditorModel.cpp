/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "EditorModel.h"
#include "BattleModel.h"
#include "BattleView.h"



EditorModel::EditorModel(BattleView* battleView, terrain* t) :
_battleView(battleView),
_terrain(t),
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
	int x0 = (int)(_terrain->_heightmap->size().m * position.x / 1024);
	int y0 = (int)(_terrain->_heightmap->size().n * position.y / 1024);

	for (int x = -5; x <= 5; ++x)
		for (int y = -5; y <= 5; ++y)
		{
			float h = _terrain->_heightmap->get_height(x0 + x, y0 + y);
			_terrain->_heightmap->set_height(x0 + x, y0 + y, fmaxf(0.1f, h + delta));
		}

	_terrain->update_heights(bounds2_from_center(position, 25));
	_battleView->UpdateTerrainTrees(bounds2_from_center(position, 25));
}


void EditorModel::EditWater(glm::vec2 position, bool value)
{
	int x0 = (int)(512 * position.x / 1024);
	int y0 = (int)(512 * position.y / 1024);

	image* map = _terrain->_heightmap->_map;

	for (int x = -5; x <= 5; ++x)
		for (int y = -5; y <= 5; ++y)
		{
			glm::vec4 c = map->get_pixel(x, y);
			c.b = value ? 1 : 0;
			map->set_pixel(x0 + x, y0 - y, c);
		}

	_terrain->update_heights(bounds2_from_center(position, 25));
	_battleView->UpdateTerrainTrees(bounds2_from_center(position, 25));
}


void EditorModel::EditTrees(glm::vec2 position, bool value)
{
	int x0 = (int)(512 * position.x / 1024);
	int y0 = (int)(512 * position.y / 1024);

	image* map = _terrain->_heightmap->_map;

	for (int x = -5; x <= 5; ++x)
		for (int y = -5; y <= 5; ++y)
		{
			glm::vec4 c = map->get_pixel(x, y);
			c.g = value ? 1 : 0;
			map->set_pixel(x0 + x, y0 - y, c);
		}

	_terrain->_forest->load(*map);
	_battleView->UpdateTerrainTrees(bounds2_from_center(position, 25));
}
