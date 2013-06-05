/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "OpenWarSurface.h"
#include "SoundPlayer.h"
#include "SimulationState.h"
#include "SimulationRules.h"
#include "BattleModel.h"
#include "BattleGesture.h"
#include "TerrainGesture.h"
#include "ButtonView.h"
#include "ButtonGesture.h"
#include "EditorGesture.h"



static SimulationState* LoadSimulationState()
{
	SimulationState* result = new SimulationState();

	NSString* prefix = @"/Users/nicke/Desktop/Map/Map1";

	image* map = new image(512, 512);

	image fords([NSString stringWithFormat:@"%@-Fords.png", prefix]);
	image forest([NSString stringWithFormat:@"%@-Forest.png", prefix]);
	image water([NSString stringWithFormat:@"%@-Water.png", prefix]);
	image height([NSString stringWithFormat:@"%@-Height.png", prefix]);

	for (int x = 0; x < 512; ++x)
		for (int y = 0; y < 512; ++y)
		{
			glm::vec4 c;
			c.r = fords.get_pixel(x, y).r;
			c.g = forest.get_pixel(x, y).r;
			c.b = water.get_pixel(x, y).r;
			c.a = height.get_pixel(x, y).r;
			map->set_pixel(x, y, c);
		}

	result->map = map;
	result->height = new heightmap(bounds2f(0, 0, 1024, 1024), map);

	result->AddUnit(Player1, 80, SimulationState::GetDefaultUnitStats(UnitPlatformSam, UnitWeaponKata), glm::vec2(500, 300));
	result->AddUnit(Player2, 80, SimulationState::GetDefaultUnitStats(UnitPlatformSam, UnitWeaponKata), glm::vec2(500, 700));

	return result;
}


OpenWarSurface::OpenWarSurface(glm::vec2 size, float pixelDensity) : Surface(size, pixelDensity),
_mode(Mode::Editing),
_simulationState(nullptr),
_simulationRules(nullptr),
_battleModel(nullptr),
_editorModel(nullptr),
_terrain(nullptr),
_battleView(nullptr),
_buttonsTopLeft(nullptr),
_buttonsTopRight(nullptr),
_terrainGesture(nullptr),
_battleGesture(nullptr),
_editorGesture(nullptr),
_buttonGesture(nullptr),
_buttonRendering(nullptr),
_buttonItemHand(nullptr),
_buttonItemPaint(nullptr),
_buttonItemErase(nullptr),
_buttonItemSmear(nullptr),
_buttonItemHills(nullptr),
_buttonItemWater(nullptr),
_buttonItemTrees(nullptr)
{
	renderers::init();
	BattleRendering::Initialize();
	SoundPlayer::Initialize();

	_simulationState = LoadSimulationState();

	_simulationRules = new SimulationRules(_simulationState);
	_simulationRules->currentPlayer = Player1;

	_terrain = new terrain(_simulationState->height, _simulationState->map, pixelDensity > 1);

	_battleModel = new BattleModel(_simulationState);
	_battleModel->_player = Player1;
	_battleModel->Initialize(_simulationState);

	_battleView = new BattleView(this, _battleModel, _terrain, Player1);
	_battleView->Initialize(_simulationState);

	_buttonRendering = new ButtonRendering(pixelDensity);
	_buttonsTopLeft = new ButtonView(this, _buttonRendering, ButtonAlignment::TopLeft);
	_buttonsTopRight = new ButtonView(this, _buttonRendering, ButtonAlignment::TopRight);

	_buttonGesture = new ButtonGesture();
	_buttonGesture->buttonViews.push_back(_buttonsTopLeft);
	_buttonGesture->buttonViews.push_back(_buttonsTopRight);

	_battleGesture = new BattleGesture(_battleView);
	_terrainGesture = new TerrainGesture(_battleView);

	_editorModel = new EditorModel(_battleView, _terrain);
	_editorGesture = new EditorGesture(_battleView, _editorModel);

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

	UpdateButtonsAndGestures();
}


OpenWarSurface::~OpenWarSurface()
{
}


void OpenWarSurface::ScreenSizeChanged()
{
	bounds2f viewport = bounds2f(0, 0, GetSize());
	_battleView->SetViewport(viewport);
	_buttonsTopLeft->SetViewport(viewport);
	_buttonsTopRight->SetViewport(viewport);
}


void OpenWarSurface::Update(double secondsSinceLastUpdate)
{
	if (_mode == Mode::Playing)
	{
		_simulationRules->AdvanceTime((float)secondsSinceLastUpdate);
		UpdateSoundPlayer();
	}
	_battleView->Update(secondsSinceLastUpdate);
}


void OpenWarSurface::Render()
{
	glClearColor(0.9137f, 0.8666f, 0.7647f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

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

	for (UnitMarker* unitMarker : _battleView->GetBoardModel()->_unitMarkers)
	{
		Unit* unit = unitMarker->_unit;
		if (_simulationState->GetUnit(unit->unitId) != 0 && glm::length(unit->movement.GetFinalDestination() - unit->state.center) > 4.0f)
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

	SoundPlayer::singleton->UpdateFighting(_simulationState->IsMelee());
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
	_simulationState->time = 0; // TODO: reload & reset simlation state
	_mode = Mode::Editing;
	UpdateButtonsAndGestures();
}


void OpenWarSurface::SetEditorMode(EditorMode editorMode)
{
	_editorModel->editorMode = editorMode;
	UpdateButtonsAndGestures();
}


void OpenWarSurface::SetEditorFeature(EditorFeature editorFeature)
{
	_editorModel->editorFeature = editorFeature;
	UpdateButtonsAndGestures();
}


void OpenWarSurface::UpdateButtonsAndGestures()
{
	_buttonItemHand->SetDisabled(_mode == Mode::Playing);
	_buttonItemPaint->SetDisabled(_mode == Mode::Playing);
	_buttonItemErase->SetDisabled(_mode == Mode::Playing);
	_buttonItemSmear->SetDisabled(_mode == Mode::Playing);
	_buttonItemHills->SetDisabled(_mode == Mode::Playing);
	_buttonItemWater->SetDisabled(_mode == Mode::Playing);
	_buttonItemTrees->SetDisabled(_mode == Mode::Playing);

	_buttonItemHand->SetSelected(_editorModel->editorMode == EditorMode::Hand);
	_buttonItemPaint->SetSelected(_editorModel->editorMode == EditorMode::Paint);
	_buttonItemErase->SetSelected(_editorModel->editorMode == EditorMode::Erase);
	_buttonItemSmear->SetSelected(_editorModel->editorMode == EditorMode::Smear);
	_buttonItemHills->SetSelected(_editorModel->editorFeature == EditorFeature::Hills);
	_buttonItemWater->SetSelected(_editorModel->editorFeature == EditorFeature::Water);
	_buttonItemTrees->SetSelected(_editorModel->editorFeature == EditorFeature::Trees);

	_battleGesture->SetEnabled(_mode == Mode::Playing);
	_terrainGesture->SetEnabled(_mode == Mode::Playing || _editorModel->editorMode == EditorMode::Hand);
	_editorGesture->SetEnabled(_mode == Mode::Editing && _editorModel->editorMode != EditorMode::Hand);

	_buttonsTopRight->Reset();
	switch (_mode)
	{
		case Mode::Editing:
			if (_simulationState->time != 0)
				_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonRendering->buttonIconRewind)->SetAction([this](){ ClickedRewind(); });
			_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonRendering->buttonIconPlay)->SetAction([this](){ ClickedPlay(); });
			break;

		case Mode::Playing:
			_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonRendering->buttonIconPause)->SetAction([this](){ ClickedPause(); });
			break;
	}
}
