/*
#include <SDL2/SDL.h>

#include "OpenWarSurface.h"
#include "Window.h"
#include "BattleScript.h"
#include "BillboardTerrainForest.h"



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
	{
		battleScript->GetBattleModel()->terrainForest = new BillboardTerrainForest();
	}

	return battleScript;
}*/

 
int main(int argc, char *argv[])
{
    /*
	SDL_Init(SDL_INIT_EVERYTHING);

	Window* window = new Window();

	OpenWarSurface* surface = new OpenWarSurface(glm::vec2(640, 480), 1);
	window->SetSurface(surface);
	surface->Reset(CreateBattleScript());

	while (!Window::IsDone())
		Window::ProcessEvents();

	SDL_Quit();
     */
    
	return 0;
}
