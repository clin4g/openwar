// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef OpenWarSurface_H
#define OpenWarSurface_H

#include "../Library/ViewCore/Surface.h"
#include "TerrainView/EditorModel.h"

class BattleGesture;
class BattleModel;
class BattleScript;
class BattleView;
class ButtonGesture;
class ButtonItem;
class ButtonRendering;
class ButtonView;
class BattleSimulator;
class EditorGesture;
class GradientLineRenderer;
class SmoothTerrainSurfaceRenderer;
class TerrainGesture;
class TiledTerrainSurfaceRenderer;


class OpenWarSurface : public Surface
{
public: // TODO: just testing
	enum class Mode { None, Editing, Playing };
	Mode _mode;

	BattleScript* _battleScript;
	BattleView* _battleView;

	renderers* _renderers;
	ButtonRendering* _buttonRendering;

	EditorModel* _editorModel;

	ButtonView* _buttonsTopLeft;
	ButtonView* _buttonsTopRight;

	TerrainGesture* _terrainGesture;
	BattleGesture* _battleGesture;
	EditorGesture* _editorGesture;
	ButtonGesture* _buttonGesture;

	ButtonItem* _buttonItemHand;
	ButtonItem* _buttonItemPaint;
	ButtonItem* _buttonItemErase;
	ButtonItem* _buttonItemSmear;
	ButtonItem* _buttonItemHills;
	ButtonItem* _buttonItemTrees;
	ButtonItem* _buttonItemWater;
	ButtonItem* _buttonItemFords;

	GradientLineRenderer* _scriptHintRenderer;

public:
	OpenWarSurface(glm::vec2 size, float pixelDensity);
	virtual ~OpenWarSurface();

	void Reset(BattleScript* battleScript);

	virtual void ScreenSizeChanged();
	virtual void Update(double secondsSinceLastUpdate);

	virtual bool NeedsRender() const;
	virtual void Render();

	virtual void MouseEnter(glm::vec2 position);
	virtual void MouseHover(glm::vec2 position);
	virtual void MouseLeave(glm::vec2 position);

private:
	void UpdateSoundPlayer();

	void ClickedPlay();
	void ClickedPause();

	void SetEditorMode(EditorMode editorMode);
	void SetEditorFeature(TerrainFeature editorFeature);

	void UpdateButtonsAndGestures();
};


#endif
