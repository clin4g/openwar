// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "EditorModel.h"
#include "BattleModel.h"
#include "BattleView.h"



EditorModel::EditorModel(BattleView* battleView, SmoothTerrainRenderer* terrainRendering) :
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
	bounds2f bounds = _terrainRendering->GetTerrainModel()->EditHills(position, 25, value ? 0.5 : -0.5);
	_terrainRendering->UpdateHeights(bounds);
	_battleView->UpdateTerrainTrees(bounds);
}


void EditorModel::EditWater(glm::vec2 position, bool value)
{
	bounds2f bounds = _terrainRendering->GetTerrainModel()->EditWater(position, 15, value ? 0.5 : -0.5);
	_terrainRendering->UpdateHeights(bounds);
	_battleView->UpdateTerrainTrees(bounds);
}


void EditorModel::EditTrees(glm::vec2 position, bool value)
{
	bounds2f bounds = _terrainRendering->GetTerrainModel()->EditTrees(position, 15, value ? 0.5 : -0.5);
	_terrainRendering->UpdateMapTexture();
	_battleView->UpdateTerrainTrees(bounds);
}
