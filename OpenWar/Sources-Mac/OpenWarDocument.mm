// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#import "OpenWarDocument.h"
#include "BattleModel.h"
#include "BattleView.h"
#include "OpenWarSurface.h"
#include "BattleScript.h"
#include "BillboardTerrainForest.h"
#include "SmoothTerrainWater.h"
#include "SmoothTerrainSky.h"


@implementation OpenWarDocument
{
	OpenWarSurface* _surface;
	NSString* _sourceTypeName;
	NSData* _sourceData;
	NSURL* _sourceURL;
	NSURL* _sourceDirectory;
}


- (id)init
{
	//NSLog(@"OpenWarDocument init");
    self = [super init];
    if (self)
	{
	}
    return self;
}


- (id)initWithType:(NSString *)typeName error:(NSError **)outError
{
	//NSLog(@"OpenWarDocument initWithType:\"%@\" error:<outError>", typeName);

	return [super initWithType:typeName error:outError];
}


- (id)initWithContentsOfURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError **)outError
{
	//NSLog(@"OpenWarDocument initWithContentsOfURL:\"%@\" ofType:\"%@\" error:<outError>", url, typeName);

	_sourceURL = [url retain];
	_sourceDirectory = [[url URLByDeletingLastPathComponent] retain];

	return [super initWithContentsOfURL:url ofType:typeName error:outError];
}



- (id)initForURL:(NSURL *)urlOrNil withContentsOfURL:(NSURL *)contentsURL ofType:(NSString *)typeName error:(NSError **)outError
{
	//NSLog(@"OpenWarDocument initForURL:\"%@\" withContentsOfURL:\"%@\" ofType:\"%@\" error:<outError>", urlOrNil, contentsURL, typeName);

	_sourceURL = [contentsURL retain];
	_sourceDirectory = [[contentsURL URLByDeletingLastPathComponent] retain];

	return [super initForURL:urlOrNil withContentsOfURL:contentsURL ofType:typeName error:outError];
}



- (void)setFileURL:(NSURL *)url
{
	//NSLog(@"OpenWarDocument setFileURL:%@", url);

	[super setFileURL:url];
}



- (NSString *)windowNibName
{
    return @"OpenWarDocument";
}


- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
	//NSLog(@"OpenWarDocument windowControllerDidLoadNib:%@", aController);

    [super windowControllerDidLoadNib:aController];
}



+ (BOOL)autosavesInPlace
{
    return YES;
}


- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{
	//NSLog(@"OpenWarDocument dataOfType:\"%@\" error:<outError>", typeName);

	if (_surface != nullptr)
	{
		SmoothTerrainSurface* terrainSurfaceModelSmooth = dynamic_cast<SmoothTerrainSurface*>(_surface->_battleScript->GetBattleModel()->terrainSurface);
		if ([typeName isEqualToString:@"SmoothMap"] && terrainSurfaceModelSmooth != nullptr)
		{
			terrainSurfaceModelSmooth->SaveHeightmapToImage();
			return ConvertImageToTiff(terrainSurfaceModelSmooth->GetMap());
		}
	}

	return nil;
}


- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
	//NSLog(@"readFromData:[%d] ofType:\"%@\" error:<outError>", (int)data.length, typeName);

	_sourceTypeName = [typeName retain];
	_sourceData = [data retain];

    return YES;
}


#pragma mark -


- (void)reload:(id)sender
{
	if ([_sourceTypeName isEqualToString:@"Script"])
	{
		[_sourceData release];
		_sourceData = [[NSData dataWithContentsOfURL:_sourceURL] retain];

		BattleScript* battlescript = [self createBattleScriptFromScript:_sourceData];
		_surface->Reset(battlescript);
	}
}



#pragma mark SurfaceFactory


- (Surface*)createSurfaceWithSize:(glm::vec2)size forSurfaceView:(SurfaceView*)surfaceView pixelDensity:(float)pixelDensity;
{
	//NSLog(@"createSurfaceWithSize:vec2(%f, %f) forSurfaceView:%@ pixelDensity:%f", size.x, size.y, surfaceView, pixelDensity);

	_surface = new OpenWarSurface(size, pixelDensity);

	BattleScript* battlescript = nullptr;

	if (_sourceData == nil)
	{
		/*NSString* path = [[NSBundle mainBundle] pathForResource:@"DefaultMap" ofType:@"tiff" inDirectory:@"Maps"];
		NSData* data = [NSData dataWithContentsOfFile:path];
		battleModel = [self createBattleModelFromSmoothMap:data];*/

		NSString* path = [[NSBundle mainBundle] pathForResource:@"DefaultMap" ofType:@"lua" inDirectory:@"Maps"];
		_sourceDirectory = [[[NSURL fileURLWithPath:path] URLByDeletingLastPathComponent] retain];
		NSData* data = [NSData dataWithContentsOfFile:path];
		battlescript = [self createBattleScriptFromScript:data];
	}
	else if ([_sourceTypeName isEqualToString:@"SmoothMap"])
	{
		battlescript = [self createBattleScriptFromSmoothMap:_sourceData];
	}
	else if ([_sourceTypeName isEqualToString:@"Script"])
	{
		battlescript = [self createBattleScriptFromScript:_sourceData];
	}

	_surface->Reset(battlescript);

	return _surface;
}


- (BattleScript*)createBattleScriptFromSmoothMap:(NSData*)smoothMap
{
	BattleScript* battleScript = new BattleScript();

	BattleModel* battleModel = battleScript->GetBattleModel();

	image* map = ConvertTiffToImage(smoothMap);

	battleModel->terrainSurface = new SmoothTerrainSurface(bounds2f(0, 0, 1024, 1024), map);
	battleModel->terrainForest = new BillboardTerrainForest();
	battleModel->terrainWater = new SmoothTerrainWater(bounds2f(0, 0, 1024, 1024), map);
	battleModel->terrainSky = new SmoothTerrainSky();

	return battleScript;
}


- (BattleScript*)createBattleScriptFromScript:(NSData*)script
{
	BattleScript* battleScript = new BattleScript();

	battleScript->SetGlobalNumber("openwar_seed", 0);
	battleScript->SetGlobalString("openwar_script_directory", _sourceDirectory.filePathURL.path.UTF8String);
	battleScript->Execute((const char*)script.bytes, script.length);

	if (battleScript->GetBattleModel()->terrainForest == nullptr)
	{
		battleScript->GetBattleModel()->terrainForest = new BillboardTerrainForest();
	}

	return battleScript;
}


@end
