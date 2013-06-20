// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#import "OpenWarDocument.h"
#include "BattleModel.h"
#include "BattleView.h"
#include "OpenWarSurface.h"
#include "BattleScript.h"
#include "BillboardTerrainForest.h"


@implementation OpenWarDocument
{
	OpenWarSurface* _surface;
	NSString* _sourceTypeName;
	NSData* _sourceData;
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

	_sourceDirectory = [[url URLByDeletingLastPathComponent] retain];

	return [super initWithContentsOfURL:url ofType:typeName error:outError];
}



- (id)initForURL:(NSURL *)urlOrNil withContentsOfURL:(NSURL *)contentsURL ofType:(NSString *)typeName error:(NSError **)outError
{
	//NSLog(@"OpenWarDocument initForURL:\"%@\" withContentsOfURL:\"%@\" ofType:\"%@\" error:<outError>", urlOrNil, contentsURL, typeName);

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
		SmoothTerrainSurface* terrainSurfaceModelSmooth = dynamic_cast<SmoothTerrainSurface*>(_surface->_battleSimulator->GetBattleModel()->terrainSurfaceModel);
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


#pragma mark SurfaceFactory


- (Surface*)createSurfaceWithSize:(glm::vec2)size forSurfaceView:(SurfaceView*)surfaceView pixelDensity:(float)pixelDensity;
{
	//NSLog(@"createSurfaceWithSize:vec2(%f, %f) forSurfaceView:%@ pixelDensity:%f", size.x, size.y, surfaceView, pixelDensity);

	_surface = new OpenWarSurface(size, pixelDensity);

	BattleModel* battleModel = nullptr;

	if (_sourceData == nil)
	{
		NSString* path = [[NSBundle mainBundle] pathForResource:@"DefaultMap" ofType:@"tiff" inDirectory:@"Maps"];
		NSData* data = [NSData dataWithContentsOfFile:path];
		battleModel = [self createBattleModelFromSmoothMap:data];
	}
	else if ([_sourceTypeName isEqualToString:@"SmoothMap"])
	{
		battleModel = [self createBattleModelFromSmoothMap:_sourceData];
	}
	else if ([_sourceTypeName isEqualToString:@"Script"])
	{
		battleModel = [self createBattleModelFromScript:_sourceData];
	}

	_surface->Reset(battleModel);

	return _surface;
}


- (BattleModel*)createBattleModelFromSmoothMap:(NSData*)smoothMap
{
	BattleModel* battleModel = new BattleModel();

	battleModel->terrainSurfaceModel = new SmoothTerrainSurface(bounds2f(0, 0, 1024, 1024), ConvertTiffToImage(smoothMap));
	battleModel->terrainForest = new BillboardTerrainForest();

	return battleModel;
}


- (BattleModel*)createBattleModelFromScript:(NSData*)script
{
	BattleModel* battleModel = new BattleModel();

	BattleScript* battleScript = new BattleScript(battleModel, _sourceDirectory.filePathURL.path.UTF8String, (const char*)script.bytes, script.length);

	if (battleModel->terrainForest == nullptr)
	{
		battleModel->terrainForest = new BillboardTerrainForest();
	}

	delete battleScript;

	return battleModel;
}


@end
