// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "EditorModel.h"
#include "../BattleModel/BattleModel.h"
#include "../BattleView/BattleView.h"
#include "../SmoothTerrain/SmoothTerrainWater.h"



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
	float radius = feature == TerrainFeature::Hills ? 48 : 16;
	SmoothTerrainSurface* terrainSurface = _terrainSurfaceRenderer->GetTerrainSurfaceModel();
	bounds2f bounds = terrainSurface->Paint(feature, position, radius, value ? 0.4f : -0.4f);

	_terrainSurfaceRenderer->UpdateChanges(bounds);
	_battleView->UpdateTerrainTrees(bounds);
	_battleView->GetBattleModel()->terrainWater->Update();
}


void EditorModel::SmearReset(TerrainFeature feature, glm::vec2 position)
{
	SmoothTerrainSurface* terrainSurface = _terrainSurfaceRenderer->GetTerrainSurfaceModel();
	terrainSurface->Extract(position, _brush);
	_brushPosition = position;
	_brushDistance = 0;
}


void EditorModel::SmearPaint(TerrainFeature feature, glm::vec2 position)
{
	SmoothTerrainSurface* terrainSurface = _terrainSurfaceRenderer->GetTerrainSurfaceModel();

	_brushDistance += glm::distance(_brushPosition, position);
	_brushPosition = position;
	while (_brushDistance > 2.0f)
	{
		terrainSurface->Extract(position, _mixer);

		glm::ivec2 size = _brush->size();
		for (int x = 0; x < size.x; ++x)
			for (int y = 0; y < size.y; ++y)
			{
				glm::vec4 c = _brush->get_pixel(x, y);
				glm::vec4 m = _mixer->get_pixel(x, y);
				c = glm::mix(c, m, 0.1f);
				_brush->set_pixel(x, y, c);
			}

		_brushDistance -= 2.0f;
	}

	bounds2f bounds = terrainSurface->Paint(feature, position, _brush, 0.5f);

	_terrainSurfaceRenderer->UpdateChanges(bounds);
	_battleView->UpdateTerrainTrees(bounds);
	_battleView->GetBattleModel()->terrainWater->Update();
}
