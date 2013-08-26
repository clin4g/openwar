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
_terrainFeature(TerrainFeature::Hills),
_brush(nullptr)
{
	_brush = new image(48, 48);
	_mixer = new image(48, 48);
}


void EditorModel::ToolBegan(glm::vec2 position)
{
	switch (_editorMode)
	{
		case EditorMode::Smear:
			SmearReset(_terrainFeature, position);
			break;

		case EditorMode::Paint:
			Paint(_terrainFeature, position, 1);
			break;

		case EditorMode::Erase:
			Paint(_terrainFeature, position, 0);
			break;

		default:
			break;
	}
}


void EditorModel::ToolMoved(glm::vec2 position)
{
	switch (_editorMode)
	{
		case EditorMode::Smear:
			SmearPaint(_terrainFeature, position);
			break;

		case EditorMode::Paint:
			Paint(_terrainFeature, position, 1);
			break;

		case EditorMode::Erase:
			Paint(_terrainFeature, position, 0);
			break;

		default:
			break;
	}
}


void EditorModel::ToolEnded(glm::vec2 position)
{
	switch (_editorMode)
	{
		case EditorMode::Smear:
			SmearPaint(_terrainFeature, position);
			break;

		case EditorMode::Paint:
			Paint(_terrainFeature, position, 1);
			break;

		case EditorMode::Erase:
			Paint(_terrainFeature, position, 0);
			break;

		default:
			break;
	}
}


void EditorModel::Paint(TerrainFeature feature, glm::vec2 position, bool value)
{
	SmoothTerrainSurface* terrainSurface = _terrainSurfaceRenderer->GetTerrainSurfaceModel();
	bounds2f bounds = terrainSurface->Paint(feature, position, 25, value ? 0.5f : -0.5f);

	_terrainSurfaceRenderer->UpdateHeights(bounds);
	_terrainSurfaceRenderer->UpdateMapTexture();
	_battleView->UpdateTerrainTrees(bounds);
	_battleView->GetBattleModel()->terrainWater->Update();
}


void EditorModel::SmearReset(TerrainFeature feature, glm::vec2 position)
{
	SmoothTerrainSurface* terrainSurface = _terrainSurfaceRenderer->GetTerrainSurfaceModel();
	terrainSurface->Extract(position, _brush);
}


void EditorModel::SmearPaint(TerrainFeature feature, glm::vec2 position)
{
	SmoothTerrainSurface* terrainSurface = _terrainSurfaceRenderer->GetTerrainSurfaceModel();

	glm::ivec2 size = _brush->size();
	terrainSurface->Extract(position, _mixer);
	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
		{
			glm::vec4 c = _brush->get_pixel(x, y);
			glm::vec4 m = _mixer->get_pixel(x, y);
			c = glm::mix(c, m, 0.15f * m.a);
			_brush->set_pixel(x, y, c);
		}

	bounds2f bounds = terrainSurface->Paint(feature, position, _brush, 0.5f);

	_terrainSurfaceRenderer->UpdateHeights(bounds);
	_terrainSurfaceRenderer->UpdateMapTexture();
	_battleView->UpdateTerrainTrees(bounds);
	_battleView->GetBattleModel()->terrainWater->Update();
}
