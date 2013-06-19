// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#import "OpenWarDocument.h"
#include "BattleContext.h"
#include "BattleModel.h"
#include "BattleView.h"
#include "OpenWarSurface.h"
#include "BattleScript.h"
#include "TerrainFeatureModelBillboard.h"
#include "TerrainFeatureModelMesh.h"
#include "TerrainSurfaceModelTiled.h"


@implementation OpenWarDocument
{
	OpenWarSurface* _surface;
	NSString* _sourceTypeName;
	NSData* _sourceData;
	NSURL* _sourceDirectory;
}


- (id)init
{
	NSLog(@"OpenWarDocument init");
    self = [super init];
    if (self)
	{
	}
    return self;
}


- (id)initWithType:(NSString *)typeName error:(NSError **)outError
{
	NSLog(@"OpenWarDocument initWithType:\"%@\" error:<outError>", typeName);

	return [super initWithType:typeName error:outError];
}


- (id)initWithContentsOfURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError **)outError
{
	NSLog(@"OpenWarDocument initWithContentsOfURL:\"%@\" ofType:\"%@\" error:<outError>", url, typeName);

	_sourceDirectory = [[url URLByDeletingLastPathComponent] retain];

	return [super initWithContentsOfURL:url ofType:typeName error:outError];
}



- (id)initForURL:(NSURL *)urlOrNil withContentsOfURL:(NSURL *)contentsURL ofType:(NSString *)typeName error:(NSError **)outError
{
	NSLog(@"OpenWarDocument initForURL:\"%@\" withContentsOfURL:\"%@\" ofType:\"%@\" error:<outError>", urlOrNil, contentsURL, typeName);

	_sourceDirectory = [[contentsURL URLByDeletingLastPathComponent] retain];

	return [super initForURL:urlOrNil withContentsOfURL:contentsURL ofType:typeName error:outError];
}



- (void)setFileURL:(NSURL *)url
{
	NSLog(@"OpenWarDocument setFileURL:%@", url);

	[super setFileURL:url];
}



- (NSString *)windowNibName
{
    return @"OpenWarDocument";
}


- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
	NSLog(@"OpenWarDocument windowControllerDidLoadNib:%@", aController);

    [super windowControllerDidLoadNib:aController];
}



+ (BOOL)autosavesInPlace
{
    return YES;
}


- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{
	NSLog(@"OpenWarDocument dataOfType:\"%@\" error:<outError>", typeName);

	if (_surface != nullptr)
	{
		TerrainSurfaceModelSmooth* terrainSurfaceModelSmooth = dynamic_cast<TerrainSurfaceModelSmooth*>(_surface->_battleContext->terrainSurfaceModel);
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
	NSLog(@"readFromData:[%d] ofType:\"%@\" error:<outError>", (int)data.length, typeName);

	_sourceTypeName = [typeName retain];
	_sourceData = [data retain];

    return YES;
}


#pragma mark SurfaceFactory


- (Surface*)createSurfaceWithSize:(glm::vec2)size forSurfaceView:(SurfaceView*)surfaceView pixelDensity:(float)pixelDensity;
{
	NSLog(@"createSurfaceWithSize:vec2(%f, %f) forSurfaceView:%@ pixelDensity:%f", size.x, size.y, surfaceView, pixelDensity);

	_surface = new OpenWarSurface(size, pixelDensity);

	BattleContext* battleContext = nullptr;

	if (_sourceData == nil)
	{
		NSString* path = [[NSBundle mainBundle] pathForResource:@"DefaultMap" ofType:@"tiff" inDirectory:@"Maps"];
		NSData* data = [NSData dataWithContentsOfFile:path];
		battleContext = [self createBattleContextFromSmoothMap:data];
	}
	else if ([_sourceTypeName isEqualToString:@"SmoothMap"])
	{
		battleContext = [self createBattleContextFromSmoothMap:_sourceData];
	}
	else if ([_sourceTypeName isEqualToString:@"Script"])
	{
		battleContext = [self createBattleContextFromScript:_sourceData];
	}

	_surface->Reset(battleContext);

	return _surface;
}


- (BattleContext*)createBattleContextFromSmoothMap:(NSData*)smoothMap
{
	BattleContext* battleContext = new BattleContext();

	battleContext->billboardTextureAtlas = new BillboardModel();
	battleContext->terrainFeatureModelBillboard = new TerrainFeatureModelBillboard();
	battleContext->terrainFeatureModelMesh = new TerrainFeatureModelMesh();

	battleContext->terrainSurfaceModel = new TerrainSurfaceModelSmooth(bounds2f(0, 0, 1024, 1024), ConvertTiffToImage(smoothMap));

	battleContext->simulationState = new SimulationState();
	battleContext->simulationState->terrainSurfaceModel = battleContext->terrainSurfaceModel;

	battleContext->simulationRules = new SimulationRules(battleContext->simulationState);
	battleContext->simulationRules->currentPlayer = Player1;

	battleContext->battleModel = new BattleModel(battleContext);
	battleContext->battleModel->_player = Player1;
	battleContext->battleModel->Initialize(battleContext->simulationState);

	return battleContext;
}


- (BattleContext*)createBattleContextFromScript:(NSData*)script
{
	BattleContext* battleContext = new BattleContext();

	battleContext->billboardTextureAtlas = new BillboardModel();
	battleContext->terrainFeatureModelBillboard = new TerrainFeatureModelBillboard();
	battleContext->terrainFeatureModelMesh = new TerrainFeatureModelMesh();

	battleContext->battleScript = new BattleScript(battleContext, _sourceDirectory.filePathURL.path.UTF8String, (const char*)script.bytes, script.length);

	if (battleContext->simulationState == nullptr)
	{
		battleContext->simulationState = new SimulationState();
		battleContext->simulationState->terrainSurfaceModel = battleContext->terrainSurfaceModel;
	}

	if (battleContext->simulationRules == nullptr)
	{
		battleContext->simulationRules = new SimulationRules(battleContext->simulationState);
		battleContext->simulationRules->currentPlayer = Player1;
	}

	if (battleContext->battleModel == nullptr)
	{
		battleContext->battleModel = new BattleModel(battleContext);
		battleContext->battleModel->_player = Player1;
		battleContext->battleModel->Initialize(battleContext->simulationState);
	}

	return battleContext;
}


@end
