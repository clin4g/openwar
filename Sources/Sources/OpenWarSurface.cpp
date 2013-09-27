// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "OpenWarSurface.h"
#include "BattleModel/BattleModel.h"
#include "BattleView/BattleGesture.h"
#include "BattleScript.h"
#include "../Library/ViewExtra/ButtonView.h"
#include "../Library/ViewExtra/ButtonGesture.h"
#include "TerrainView/EditorGesture.h"
#include "Simulator/BattleSimulator.h"
#include "../Library/Audio/SoundPlayer.h"
#include "TerrainView/TerrainGesture.h"
#include "TerrainSurface/TiledTerrainSurface.h"
#include "BattleModel/UnitCounter.h"
#include "../Library/Renderers/GradientRenderer.h"



OpenWarSurface::OpenWarSurface(glm::vec2 size, float pixelDensity) : Surface(size, pixelDensity),
_mode(Mode::None),
_battleScript(nullptr),
_battleView(nullptr),
_renderers(nullptr),
_buttonRendering(nullptr),
_editorModel(nullptr),
_buttonsTopLeft(nullptr),
_buttonsTopRight(nullptr),
_terrainGesture(nullptr),
_battleGesture(nullptr),
_editorGesture(nullptr),
_buttonGesture(nullptr),
_buttonItemHand(nullptr),
_buttonItemPaint(nullptr),
_buttonItemErase(nullptr),
_buttonItemSmear(nullptr),
_buttonItemHills(nullptr),
_buttonItemTrees(nullptr),
_buttonItemWater(nullptr),
_buttonItemFords(nullptr),
_scriptHintRenderer(nullptr)
{
	SoundPlayer::Initialize();
	SoundPlayer::singleton->Pause();

	_renderers = renderers::singleton = new renderers();
	_buttonRendering = new ButtonRendering(_renderers, pixelDensity);

	_buttonsTopLeft = new ButtonView(this, _buttonRendering, ButtonAlignment::TopLeft);
	_buttonsTopRight = new ButtonView(this, _buttonRendering, ButtonAlignment::TopRight);

	_buttonGesture = new ButtonGesture();
	_buttonGesture->buttonViews.push_back(_buttonsTopLeft);
	_buttonGesture->buttonViews.push_back(_buttonsTopRight);

	ButtonArea* toolButtonArea = _buttonsTopLeft->AddButtonArea(4);
	_buttonItemHand = toolButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolHand);
	_buttonItemSmear = toolButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolSmear);
	_buttonItemPaint = toolButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolPaint);
	_buttonItemErase = toolButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolErase);

	ButtonArea* featureButtonArea = _buttonsTopLeft->AddButtonArea(4);
	_buttonItemHills = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolHills);
	_buttonItemTrees = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolTrees);
	_buttonItemWater = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolWater);
	_buttonItemFords = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolFords);

	_buttonItemHand->SetAction([this](){ SetEditorMode(EditorMode::Hand); });
	_buttonItemPaint->SetAction([this](){ SetEditorMode(EditorMode::Paint); });
	_buttonItemErase->SetAction([this](){ SetEditorMode(EditorMode::Erase); });
	_buttonItemSmear->SetAction([this](){ SetEditorMode(EditorMode::Smear); });
	_buttonItemHills->SetAction([this](){ SetEditorFeature(TerrainFeature::Hills); });
	_buttonItemTrees->SetAction([this](){ SetEditorFeature(TerrainFeature::Trees); });
	_buttonItemWater->SetAction([this](){ SetEditorFeature(TerrainFeature::Water); });
	_buttonItemFords->SetAction([this](){ SetEditorFeature(TerrainFeature::Fords); });

	_buttonItemHand->SetKeyboardShortcut('1');
	_buttonItemSmear->SetKeyboardShortcut('2');
	_buttonItemPaint->SetKeyboardShortcut('3');
	_buttonItemErase->SetKeyboardShortcut('4');
	_buttonItemHills->SetKeyboardShortcut('5');
	_buttonItemTrees->SetKeyboardShortcut('6');
	_buttonItemWater->SetKeyboardShortcut('7');
	_buttonItemFords->SetKeyboardShortcut('8');

	UpdateButtonsAndGestures();

	_scriptHintRenderer = new GradientLineRenderer();
}


OpenWarSurface::~OpenWarSurface()
{

}


void OpenWarSurface::Reset(BattleScript* battleScript)
{
	delete _terrainGesture;
	_terrainGesture = nullptr;

	delete _battleGesture;
	_battleGesture = nullptr;

	delete _editorGesture;
	_editorGesture = nullptr;

	delete _editorModel;
	_editorModel = nullptr;

	if (_battleView != nullptr)
	{
		delete _battleView->_smoothTerrainSurface;
		_battleView->_smoothTerrainSurface = nullptr;

		delete _battleView->_terrainSurfaceRendererTiled;
		_battleView->_terrainSurfaceRendererTiled = nullptr;
	}

	delete _battleView;
	_battleView = nullptr;

	delete _battleScript;
	_battleScript = nullptr;

	/***/

	_battleScript = battleScript;

	battleScript->GetBattleModel()->bluePlayer = Player1;
	_battleView = new BattleView(this, battleScript->GetBattleModel(), _renderers);
	_battleView->_player = Player1;

	SmoothTerrainSurface* smoothTerrainSurface = dynamic_cast<SmoothTerrainSurface*>(battleScript->GetBattleModel()->terrainSurface);
	if (smoothTerrainSurface != nullptr)
	{
		_battleView->_smoothTerrainSurface = smoothTerrainSurface;
		_battleView->_smoothTerrainSurface->EnableRenderEdges();
	}

	TiledTerrainSurface* terrainSurfaceModelTiled = dynamic_cast<TiledTerrainSurface*>(battleScript->GetBattleModel()->terrainSurface);
	if (terrainSurfaceModelTiled != nullptr)
		_battleView->_terrainSurfaceRendererTiled = new TiledTerrainSurfaceRenderer(terrainSurfaceModelTiled);

	_battleView->Initialize();

	_editorModel = new EditorModel(_battleView, _battleView->_smoothTerrainSurface);
	_editorGesture = new EditorGesture(_battleView, _editorModel);

	_battleGesture = new BattleGesture(_battleView);
	_terrainGesture = new TerrainGesture(_battleView);

	_mode = Mode::Editing;
	//_mode = Mode::Playing;
	UpdateButtonsAndGestures();

	if (battleScript->GetBattleSimulator() != nullptr)
	{
		battleScript->GetBattleSimulator()->listener = _battleView;
		battleScript->GetBattleSimulator()->currentPlayer = Player1;
	}
}


void OpenWarSurface::ScreenSizeChanged()
{
	bounds2f viewport = bounds2f(0, 0, GetSize());
	if (_battleView != nullptr)
		_battleView->SetViewport(viewport);
	_buttonsTopLeft->SetViewport(viewport);
	_buttonsTopRight->SetViewport(viewport);
}


void OpenWarSurface::Update(double secondsSinceLastUpdate)
{
	if (_mode == Mode::Playing)
	{
		_battleScript->Tick(secondsSinceLastUpdate);
		UpdateSoundPlayer();
	}
	if (_battleView != nullptr)
	{
		_battleView->Update(secondsSinceLastUpdate);
		_battleView->GetBattleModel()->AnimateMarkers((float)secondsSinceLastUpdate);
	}
}


bool OpenWarSurface::NeedsRender() const
{
	return true;
}


void OpenWarSurface::Render()
{
	glClearColor(0.9137f, 0.8666f, 0.7647f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);

	if (_battleView != nullptr)
	{
		_battleView->Render();

		if (_battleScript != nullptr)
		{
			_scriptHintRenderer->Reset();
			_battleScript->RenderHints(_scriptHintRenderer);
			_scriptHintRenderer->Draw(_battleView->GetTransform());
		}
	}

	if (_battleGesture == nullptr)
		_battleGesture->RenderHints();

	_buttonsTopLeft->Render();
	_buttonsTopRight->Render();
}


void OpenWarSurface::MouseEnter(glm::vec2 position)
{
	//_battleView->ShowMouseHint(position);
}


void OpenWarSurface::MouseHover(glm::vec2 position)
{
	//_battleView->ShowMouseHint(position);
}


void OpenWarSurface::MouseLeave(glm::vec2 position)
{
	//_battleView->HideMouseHint();
}


void OpenWarSurface::UpdateSoundPlayer()
{
	if (_mode == Mode::Playing)
	{
		int horseGallop = 0;
		int horseTrot = 0;
		int fighting = 0;
		int infantryMarching = 0;
		int infantryRunning = 0;

		for (UnitCounter* unitMarker : _battleView->GetBattleModel()->_unitMarkers)
		{
			Unit* unit = unitMarker->_unit;
			if (_battleScript->GetBattleModel()->GetUnit(unit->unitId) != 0 && glm::length(unit->command.GetDestination() - unit->state.center) > 4.0f)
			{
				if (unit->stats.unitPlatform == UnitPlatformCav || unit->stats.unitPlatform == UnitPlatformGen)
				{
					if (unit->command.running)
						++horseGallop;
					else
						++horseTrot;
				}
				else
				{
					if (unit->command.running)
						++infantryRunning;
					else
						++infantryMarching;
				}
			}

			if (unit->command.meleeTarget != nullptr)
				++fighting;
		}

		SoundPlayer::singleton->UpdateInfantryWalking(infantryMarching != 0);
		SoundPlayer::singleton->UpdateInfantryRunning(infantryRunning != 0);

		SoundPlayer::singleton->UpdateCavalryWalking(horseTrot != 0);
		SoundPlayer::singleton->UpdateCavalryRunning(horseGallop != 0);

		SoundPlayer::singleton->UpdateFighting(_battleScript->GetBattleModel()->IsMelee());
	}
}


void OpenWarSurface::ClickedPlay()
{
	_mode = Mode::Playing;
	SoundPlayer::singleton->Resume();
	UpdateButtonsAndGestures();
}


void OpenWarSurface::ClickedPause()
{
	_mode = Mode::Editing;
	SoundPlayer::singleton->Pause();
	UpdateButtonsAndGestures();
}


void OpenWarSurface::SetEditorMode(EditorMode editorMode)
{
	if (_editorModel != nullptr)
	{
		_editorModel->SetEditorMode(editorMode);
		UpdateButtonsAndGestures();
	}
}


void OpenWarSurface::SetEditorFeature(TerrainFeature terrainFeature)
{
	if (_editorModel != nullptr)
	{
		_editorModel->SetTerrainFeature(terrainFeature);
		UpdateButtonsAndGestures();
	}
}


void OpenWarSurface::UpdateButtonsAndGestures()
{
	_buttonItemHand->SetDisabled(_mode != Mode::Editing);
	_buttonItemPaint->SetDisabled(_mode != Mode::Editing);
	_buttonItemErase->SetDisabled(_mode != Mode::Editing);
	_buttonItemSmear->SetDisabled(_mode != Mode::Editing);
	_buttonItemHills->SetDisabled(_mode != Mode::Editing);
	_buttonItemTrees->SetDisabled(_mode != Mode::Editing);
	_buttonItemWater->SetDisabled(_mode != Mode::Editing);
	_buttonItemFords->SetDisabled(_mode != Mode::Editing);

	if (_mode == Mode::None)
		return;

	_buttonItemHand->SetSelected(_editorModel->GetEditorMode() == EditorMode::Hand);
	_buttonItemPaint->SetSelected(_editorModel->GetEditorMode() == EditorMode::Paint);
	_buttonItemErase->SetSelected(_editorModel->GetEditorMode() == EditorMode::Erase);
	_buttonItemSmear->SetSelected(_editorModel->GetEditorMode() == EditorMode::Smear);
	_buttonItemHills->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Hills);
	_buttonItemTrees->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Trees);
	_buttonItemWater->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Water);
	_buttonItemFords->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Fords);

	_battleGesture->SetEnabled(_mode == Mode::Playing);
	_editorGesture->SetEnabled(_mode == Mode::Editing);

	_buttonsTopRight->Reset();
	switch (_mode)
	{
		case Mode::None:
			break;

		case Mode::Editing:
			_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonRendering->buttonIconPlay)->SetAction([this](){ ClickedPlay(); });
			break;

		case Mode::Playing:
			_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonRendering->buttonIconPause)->SetAction([this](){ ClickedPause(); });
			break;
	}
}
