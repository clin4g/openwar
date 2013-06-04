/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "OpenWarSurface.h"
#include "SoundPlayer.h"
#include "SoundLoader.h"
#import "SimulationState.h"
#import "SimulationRules.h"
#import "terrain.h"
#include "BattleModel.h"
#import "BattleGesture.h"
#import "TerrainGesture.h"



static SimulationState* LoadSimulationState()
{
	SimulationState* result = new SimulationState();

	NSString* mapDir = @"/Users/nicke/Projects/Samurai/Media/Maps Textures";
	NSString* mapName = @"Map1";


	image height([NSString stringWithFormat:@"%@/%@-Height.png", mapDir, mapName]);

	result->forest = new image([NSString stringWithFormat:@"%@/%@-Forest.png", mapDir, mapName]);
	result->water  = new image([NSString stringWithFormat:@"%@/%@-Water.png", mapDir, mapName]);
	result->fords  = new image([NSString stringWithFormat:@"%@/%@-Fords.png", mapDir, mapName]);
	result->height = new heightmap(bounds2f(0, 0, 1024, 1024), height, result->water, result->fords);

	result->AddUnit(Player1, 80, SimulationState::GetDefaultUnitStats(UnitPlatformSam, UnitWeaponKata),  glm::vec2(500, 300));
	result->AddUnit(Player2, 80, SimulationState::GetDefaultUnitStats(UnitPlatformSam, UnitWeaponKata),  glm::vec2(500, 700));

	return result;
}




OpenWarSurface::OpenWarSurface(glm::vec2 size, float pixelDensity) : Surface(size, pixelDensity)
{
	renderers::init();
	BattleRendering::Initialize();
	SoundPlayer::Initialize();

	_simulationState = LoadSimulationState();

	_simulationRules = new SimulationRules(_simulationState);
	_simulationRules->currentPlayer = Player1;
	//simulationRules->listener = this;

	_terrain = new terrain(_simulationState->height, _simulationState->forest, pixelDensity > 1);

	_battleModel = new BattleModel(_simulationState);
	_battleModel->_player = Player1;
	_battleModel->Initialize(_simulationState);

	_battleView = new BattleView(this, _battleModel, _terrain, Player1);
	_battleView->Initialize(_simulationState);
	_gestures.push_back(new BattleGesture(_battleView));
	_gestures.push_back(new TerrainGesture(_battleView));

}


OpenWarSurface::~OpenWarSurface()
{
}


void OpenWarSurface::ScreenSizeChanged()
{
	_battleView->SetViewport(bounds2f(0, 0, GetSize()));
}


void OpenWarSurface::Update(double secondsSinceLastUpdate)
{
	_simulationRules->AdvanceTime((float)secondsSinceLastUpdate);
	_battleView->Update(secondsSinceLastUpdate);

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


void OpenWarSurface::Render()
{
	glClearColor(0.9137f, 0.8666f, 0.7647f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	_battleView->Render();
}
