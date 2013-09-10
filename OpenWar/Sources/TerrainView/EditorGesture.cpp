// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "EditorGesture.h"
#include "BattleView.h"
#include "Touch.h"
#include "EditorModel.h"



EditorGesture::EditorGesture(BattleView* battleView, EditorModel* editorModel) :
_battleView(battleView),
_editorModel(editorModel)
{
}


void EditorGesture::Update(Surface* surface, double secondsSinceLastUpdate)
{

}


void EditorGesture::TouchBegan(Touch* touch)
{
	if (touch->GetSurface() != _battleView->GetSurface())
		return;

	if (_editorModel->GetEditorMode() == EditorMode::Hand)
	{
		bounds2f b = _battleView->GetContentBounds();
		glm::vec2 p = (TerrainPosition(touch) - b.min) / b.size();
		NSLog(@"Position: %g, %g", p.x, p.y);
		return;
	}

	if (touch->GetGesture() != nullptr || !_touches.empty())
		return;

	_editorModel->ToolBegan(TerrainPosition(touch));
	CaptureTouch(touch);
}


void EditorGesture::TouchMoved()
{
	if (!_touches.empty())
		_editorModel->ToolMoved(TerrainPosition(_touches.front()));
}


void EditorGesture::TouchEnded(Touch* touch)
{
	_editorModel->ToolEnded(TerrainPosition(touch));

}


void EditorGesture::TouchWasCancelled(Touch* touch)
{

}


glm::vec2 EditorGesture::TerrainPosition(Touch* touch)
{
	return _battleView->GetTerrainPosition3(touch->GetPosition()).xy();
}
