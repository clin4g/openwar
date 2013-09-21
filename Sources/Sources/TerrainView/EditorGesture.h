// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef EDITORGESTURE_H
#define EDITORGESTURE_H

#include "../../Library/ViewCore/Gesture.h"

class BattleView;
class EditorModel;


class EditorGesture : public Gesture
{
	BattleView* _battleView;
	EditorModel* _editorModel;

public:
	EditorGesture(BattleView* battleView, EditorModel* editorModel);

	virtual void Update(Surface* surface, double secondsSinceLastUpdate);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);

	virtual void TouchWasCancelled(Touch* touch);

private:
	glm::vec2 TerrainPosition(Touch* touch);
};


#endif
