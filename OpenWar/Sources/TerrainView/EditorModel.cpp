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
_editorMode(EditorMode::Hand),
_terrainFeature(TerrainFeature::Hills)
{
}


void EditorModel::ToolBegan(glm::vec2 position)
{
	Paint(_terrainFeature, position, _editorMode == EditorMode::Paint);
}


void EditorModel::ToolMoved(glm::vec2 position)
{
	Paint(_terrainFeature, position, _editorMode == EditorMode::Paint);
}


void EditorModel::ToolEnded(glm::vec2 position)
{
	Paint(_terrainFeature, position, _editorMode == EditorMode::Paint);
}


void EditorModel::Paint(TerrainFeature feature, glm::vec2 position, bool value)
{
	SmoothTerrainSurface* terrainSurface = _terrainSurfaceRenderer->GetTerrainSurfaceModel();
	bounds2f bounds = terrainSurface->Paint(feature, position, 25, value ? 0.5 : -0.5);
	_terrainSurfaceRenderer->UpdateHeights(bounds);
	_terrainSurfaceRenderer->UpdateMapTexture();
	_battleView->UpdateTerrainTrees(bounds);
	_battleView->GetBattleModel()->terrainWater->Update();
}
