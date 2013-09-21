// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <iostream>

#if OPENWAR_USE_GLEW
#include <GL/glew.h>
#endif

#ifdef OPENWAR_USE_SDL
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif
#endif

#include "Sources/OpenWarSurface.h"
#include "Library/ViewCore/Window.h"
#include "Sources/BattleScript.h"
#include "Sources/TerrainForest/BillboardTerrainForest.h"



#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES

static BattleScript* CreateBattleScript()
{
	NSString* path = [[NSBundle mainBundle] pathForResource:@"DefaultMap" ofType:@"lua" inDirectory:@"Maps"];
	NSURL* sourceDirectory = [[[NSURL fileURLWithPath:path] URLByDeletingLastPathComponent] retain];
	NSData* script = [NSData dataWithContentsOfFile:path];
    
	const char* directory = sourceDirectory.filePathURL.path.UTF8String;
    
	BattleScript* battleScript = new BattleScript();
	battleScript->SetGlobalNumber("openwar_seed", 0);
	battleScript->SetGlobalString("openwar_script_directory", directory);
	battleScript->AddStandardPath();
	battleScript->AddPackagePath((std::string(directory) + "/?.lua").c_str());
    
	battleScript->Execute((const char*)script.bytes, script.length);
    
	if (battleScript->GetBattleModel()->terrainForest == nullptr)
		battleScript->GetBattleModel()->terrainForest = new BillboardTerrainForest();
    
	return battleScript;
}

#else

static BattleScript* CreateBattleScript()
{
	resource script("Maps/DefaultMap.lua");
	script.load();
    
	std::string directory = resource("Maps/").path();
	std::string package_path = directory + "/?.lua";
    
	BattleScript* battleScript = new BattleScript();
	battleScript->SetGlobalNumber("openwar_seed", 0);
	battleScript->SetGlobalString("openwar_script_directory", directory.c_str());
	battleScript->AddStandardPath();
	battleScript->AddPackagePath(package_path.c_str());
    
	battleScript->Execute((const char*)script.data(), script.size());
    
	if (battleScript->GetBattleModel()->terrainForest == nullptr)
		battleScript->GetBattleModel()->terrainForest = new BillboardTerrainForest();
    
	return battleScript;
}

#endif

 
int main(int argc, char *argv[])
{
	std::cout << "Hello" << std::endl;
	std::cout << argv[0] << std::endl;

    resource::init(argv[0]);

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	Window* window = new Window();

#if OPENWAR_USE_GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "glewInit() -> " << glewGetErrorString(err) << std::endl;
		return -1;
	}
#endif
    
	OpenWarSurface* surface = new OpenWarSurface(glm::vec2(640, 480), 1);
	window->SetSurface(surface);

	surface->Reset(CreateBattleScript());
    
	while (!Window::IsDone())
		Window::ProcessEvents();

	SDL_Quit();
    
	return 0;
}
