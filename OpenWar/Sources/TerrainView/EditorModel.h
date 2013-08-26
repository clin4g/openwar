// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmoothTerrainSurfaceRenderer.h"

#ifndef EDITORMODEL_H
#define EDITORMODEL_H

class BattleView;

enum class EditorMode { Hand, Paint, Erase, Smear };


class EditorModel
{
	BattleView* _battleView;
	SmoothTerrainSurfaceRenderer* _terrainSurfaceRenderer;
	EditorMode _editorMode;
	TerrainFeature _terrainFeature;

public:

	EditorModel(BattleView* battleView, SmoothTerrainSurfaceRenderer* terrainSurfaceRenderer);

	EditorMode GetEditorMode() const { return _editorMode; }
	void SetEditorMode(EditorMode value) { _editorMode = value; }

	TerrainFeature GetTerrainFeature() const { return _terrainFeature; }
	void SetTerrainFeature(TerrainFeature value) { _terrainFeature = value; }

	void ToolBegan(glm::vec2 position);
	void ToolMoved(glm::vec2 position);
	void ToolEnded(glm::vec2 position);

private:
	void Paint(TerrainFeature feature, glm::vec2 position, bool value);
};


#endif
