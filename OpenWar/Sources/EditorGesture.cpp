/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "EditorGesture.h"
#include "BattleView.h"
#include "Touch.h"
#include "EditorModel.h"



EditorGesture::EditorGesture(BattleView* battleView, EditorModel* editorModel) :
_battleView(battleView),
_editorModel(editorModel)
{
}


void EditorGesture::Update(double secondsSinceLastUpdate)
{

}


void EditorGesture::TouchBegan(Touch* touch)
{
	if (touch->GetSurface() != _battleView->GetScreen())
		return;
	if (touch->GetGesture() != nullptr || !_touches.empty())
		return;

	CaptureTouch(touch);

	_editorModel->ToolBegan(TerrainPosition(touch));
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
