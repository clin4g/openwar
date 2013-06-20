// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "EditorModel.h"
#include "BattleModel.h"
#include "BattleView.h"
#include "SmoothTerrainWater.h"



EditorModel::EditorModel(BattleView* battleView, SmoothTerrainSurfaceRenderer* terrainSurfaceRenderer) :
_battleView(battleView),
_terrainSurfaceRenderer(terrainSurfaceRenderer),
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
	bounds2f bounds = _terrainSurfaceRenderer->GetTerrainSurfaceModel()->EditHills(position, 25, value ? 0.5 : -0.5);
	_terrainSurfaceRenderer->UpdateHeights(bounds);
	_battleView->UpdateTerrainTrees(bounds);
}


void EditorModel::EditWater(glm::vec2 position, bool value)
{
	bounds2f bounds = _terrainSurfaceRenderer->GetTerrainSurfaceModel()->EditWater(position, 15, value ? 0.5 : -0.5);
	_terrainSurfaceRenderer->UpdateHeights(bounds);
	_battleView->UpdateTerrainTrees(bounds);
	_battleView->GetBattleModel()->terrainWater->Update();
}


void EditorModel::EditTrees(glm::vec2 position, bool value)
{
	bounds2f bounds = _terrainSurfaceRenderer->GetTerrainSurfaceModel()->EditTrees(position, 15, value ? 0.5 : -0.5);
	_terrainSurfaceRenderer->UpdateMapTexture();
	_battleView->UpdateTerrainTrees(bounds);
}
