/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef OpenWarSurface_H
#define OpenWarSurface_H

#include "Surface.h"
#include "EditorModel.h"

class BattleGesture;
class BattleModel;
class BattleView;
class ButtonGesture;
class ButtonItem;
class ButtonRendering;
class ButtonView;
class EditorGesture;
class SimulationRules;
class SimulationState;
class terrain;
class TerrainGesture;


class OpenWarSurface : public Surface
{
	enum class Mode { Editing, Playing };
	Mode _mode;
	SimulationState* _simulationState;
	SimulationRules* _simulationRules;

	BattleModel* _battleModel;
	EditorModel* _editorModel;

	terrain* _terrain;
	BattleView* _battleView;
	ButtonView* _buttonsTopLeft;
	ButtonView* _buttonsTopRight;

	TerrainGesture* _terrainGesture;
	BattleGesture* _battleGesture;
	EditorGesture* _editorGesture;
	ButtonGesture* _buttonGesture;

	ButtonRendering* _buttonRendering;
	ButtonItem* _buttonItemHand;
	ButtonItem* _buttonItemPaint;
	ButtonItem* _buttonItemErase;
	ButtonItem* _buttonItemSmear;
	ButtonItem* _buttonItemHills;
	ButtonItem* _buttonItemWater;
	ButtonItem* _buttonItemTrees;

public:
	OpenWarSurface(glm::vec2 size, float pixelDensity);
	virtual ~OpenWarSurface();
    
	virtual void ScreenSizeChanged();
	virtual void Update(double secondsSinceLastUpdate);
	virtual void Render();

private:
	void UpdateSoundPlayer();

	void ClickedPlay();
	void ClickedPause();
	void ClickedRewind();

	void SetEditorMode(EditorMode editorMode);
	void SetEditorFeature(EditorFeature editorFeature);

	void UpdateButtonsAndGestures();
};


#endif
