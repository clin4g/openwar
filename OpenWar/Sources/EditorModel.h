/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "terrain.h"

#ifndef EDITORMODEL_H
#define EDITORMODEL_H

class BattleView;

enum class EditorFeature { Hills, Water, Trees };
enum class EditorMode { Hand, Paint, Erase, Smear };


class EditorModel
{
	BattleView* _battleView;
	terrain* _terrain;

public:
	EditorFeature editorFeature;
	EditorMode editorMode;

	EditorModel(BattleView* battleView, terrain* t);

	void ToolBegan(glm::vec2 position);
	void ToolMoved(glm::vec2 position);
	void ToolEnded(glm::vec2 position);

private:
	void EditHills(glm::vec2 position, bool value);
	void EditWater(glm::vec2 position, bool value);
	void EditTrees(glm::vec2 position, bool value);
};


#endif
