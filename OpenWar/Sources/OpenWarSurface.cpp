// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "OpenWarSurface.h"
#include "BattleContext.h"
#include "BattleModel.h"
#include "BattleGesture.h"
#include "BattleScript.h"
#include "ButtonView.h"
#include "ButtonGesture.h"
#include "EditorGesture.h"
#include "SimulationRules.h"
#include "SoundPlayer.h"
#include "TerrainGesture.h"
#include "TerrainSurfaceModelTiled.h"
#include "UnitCounter.h"


OpenWarSurface::OpenWarSurface(glm::vec2 size, float pixelDensity) : Surface(size, pixelDensity),
_mode(Mode::None),
_battleContext(nullptr),
_renderers(nullptr),
_battleRendering(nullptr),
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
_buttonItemWater(nullptr),
_buttonItemTrees(nullptr)
{
	SoundPlayer::Initialize();

	_renderers = renderers::singleton = new renderers();
	_battleRendering = new BattleRendering();
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

	ButtonArea* featureButtonArea = _buttonsTopLeft->AddButtonArea(3);
	_buttonItemHills = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolHills);
	_buttonItemWater = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolWater);
	_buttonItemTrees = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolTrees);

	_buttonItemHand->SetAction([this](){ SetEditorMode(EditorMode::Hand); });
	_buttonItemPaint->SetAction([this](){ SetEditorMode(EditorMode::Paint); });
	_buttonItemErase->SetAction([this](){ SetEditorMode(EditorMode::Erase); });
	_buttonItemSmear->SetAction([this](){ SetEditorMode(EditorMode::Smear); });
	_buttonItemHills->SetAction([this](){ SetEditorFeature(EditorFeature::Hills); });
	_buttonItemWater->SetAction([this](){ SetEditorFeature(EditorFeature::Water); });
	_buttonItemTrees->SetAction([this](){ SetEditorFeature(EditorFeature::Trees); });

	_buttonItemHand->SetKeyboardShortcut('1');
	_buttonItemSmear->SetKeyboardShortcut('2');
	_buttonItemPaint->SetKeyboardShortcut('3');
	_buttonItemErase->SetKeyboardShortcut('4');
	_buttonItemHills->SetKeyboardShortcut('5');
	_buttonItemWater->SetKeyboardShortcut('6');
	_buttonItemTrees->SetKeyboardShortcut('7');

	UpdateButtonsAndGestures();
}


OpenWarSurface::~OpenWarSurface()
{

}


void OpenWarSurface::Reset(BattleContext* battleContext)
{
	_battleContext = battleContext;

	_battleView = new BattleView(this, _battleContext->battleModel, _renderers, _battleRendering, Player1);
	_battleView->_player = Player1;

	battleContext->simulationRules->listener = _battleView;

	TerrainSurfaceModelSmooth* terrainSurfaceModelSmooth = dynamic_cast<TerrainSurfaceModelSmooth*>(_battleContext->battleModel->terrainSurfaceModel);
	if (terrainSurfaceModelSmooth != nullptr)
		_battleView->_terrainSurfaceRendererSmooth = new TerrainSurfaceRendererSmooth(terrainSurfaceModelSmooth, true);

	TerrainSurfaceModelTiled* terrainSurfaceModelTiled = dynamic_cast<TerrainSurfaceModelTiled*>(_battleContext->battleModel->terrainSurfaceModel);
	if (terrainSurfaceModelTiled != nullptr)
		_battleView->_terrainSurfaceRendererTiled = new TerrainSurfaceRendererTiled(terrainSurfaceModelTiled);


	_battleView->Initialize(true);

	_editorModel = new EditorModel(_battleView, _battleView->_terrainSurfaceRendererSmooth);
	_editorGesture = new EditorGesture(_battleView, _editorModel);

	_battleGesture = new BattleGesture(_battleView);
	_terrainGesture = new TerrainGesture(_battleView);


	_mode = Mode::Editing;
	UpdateButtonsAndGestures();
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
		_battleContext->simulationRules->AdvanceTime((float)secondsSinceLastUpdate);
		UpdateSoundPlayer();
	}
	if (_battleView != nullptr)
	{
		_battleView->Update(secondsSinceLastUpdate);
		_battleView->GetBattleModel()->AnimateMarkers((float)secondsSinceLastUpdate);
	}
}


void OpenWarSurface::Render()
{
	glClearColor(0.9137f, 0.8666f, 0.7647f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);

	if (_battleView != nullptr)
		_battleView->Render();

	_buttonsTopLeft->Render();
	_buttonsTopRight->Render();
}


void OpenWarSurface::UpdateSoundPlayer()
{
	int horseGallop = 0;
	int horseTrot = 0;
	int fighting = 0;
	int infantryMarching = 0;
	int infantryRunning = 0;

	for (UnitCounter* unitMarker : _battleView->GetBattleModel()->_unitMarkers)
	{
		Unit* unit = unitMarker->_unit;
		if (_battleContext->battleModel->GetUnit(unit->unitId) != 0 && glm::length(unit->movement.GetFinalDestination() - unit->state.center) > 4.0f)
		{
			if (unit->stats.unitPlatform == UnitPlatformCav || unit->stats.unitPlatform == UnitPlatformGen)
			{
				if (unit->movement.running)
					++horseGallop;
				else
					++horseTrot;
			}
			else
			{
				if (unit->movement.running)
					++infantryRunning;
				else
					++infantryMarching;
			}
		}

		if (unit->movement.target)
			++fighting;
	}

	SoundPlayer::singleton->UpdateInfantryWalking(infantryMarching != 0);
	SoundPlayer::singleton->UpdateInfantryRunning(infantryRunning != 0);

	SoundPlayer::singleton->UpdateCavalryWalking(horseTrot != 0);
	SoundPlayer::singleton->UpdateCavalryRunning(horseGallop != 0);

	SoundPlayer::singleton->UpdateFighting(_battleContext->battleModel->IsMelee());
}


void OpenWarSurface::ClickedPlay()
{
	_mode = Mode::Playing;
	UpdateButtonsAndGestures();
}


void OpenWarSurface::ClickedPause()
{
	_mode = Mode::Editing;
	UpdateButtonsAndGestures();
}


void OpenWarSurface::ClickedRewind()
{
	_battleContext->battleModel->time = 0; // TODO: reload & reset simlation state
	_mode = Mode::Editing;
	UpdateButtonsAndGestures();
}


void OpenWarSurface::SetEditorMode(EditorMode editorMode)
{
	if (_editorModel != nullptr)
	{
		_editorModel->editorMode = editorMode;
		UpdateButtonsAndGestures();
	}
}


void OpenWarSurface::SetEditorFeature(EditorFeature editorFeature)
{
	if (_editorModel != nullptr)
	{
		_editorModel->editorFeature = editorFeature;
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
	_buttonItemWater->SetDisabled(_mode != Mode::Editing);
	_buttonItemTrees->SetDisabled(_mode != Mode::Editing);

	if (_mode == Mode::None)
		return;

	_buttonItemHand->SetSelected(_editorModel->editorMode == EditorMode::Hand);
	_buttonItemPaint->SetSelected(_editorModel->editorMode == EditorMode::Paint);
	_buttonItemErase->SetSelected(_editorModel->editorMode == EditorMode::Erase);
	_buttonItemSmear->SetSelected(_editorModel->editorMode == EditorMode::Smear);
	_buttonItemHills->SetSelected(_editorModel->editorFeature == EditorFeature::Hills);
	_buttonItemWater->SetSelected(_editorModel->editorFeature == EditorFeature::Water);
	_buttonItemTrees->SetSelected(_editorModel->editorFeature == EditorFeature::Trees);

	_battleGesture->SetEnabled(_mode == Mode::Playing);
	_editorGesture->SetEnabled(_mode == Mode::Editing && _editorModel->editorMode != EditorMode::Hand);

	_buttonsTopRight->Reset();
	switch (_mode)
	{
		case Mode::None:
			break;

		case Mode::Editing:
			if (_battleContext->battleModel->time != 0)
				_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonRendering->buttonIconRewind)->SetAction([this](){ ClickedRewind(); });
			_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonRendering->buttonIconPlay)->SetAction([this](){ ClickedPlay(); });
			break;

		case Mode::Playing:
			_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonRendering->buttonIconPause)->SetAction([this](){ ClickedPause(); });
			break;
	}
}
