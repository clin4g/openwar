// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#import "OpenWarDocument.h"
#include "BattleContext.h"
#include "BattleModel.h"
#include "BattleView.h"
#include "OpenWarSurface.h"
#include "SimulationRules.h"
#include "BattleScript.h"
#include "BillboardTexture.h"
#include "TerrainSurfaceModelTiled.h"
#include "TerrainFeatureModelBillboard.h"
#include "TerrainFeatureModelMesh.h"


static NSData* ConvertImageToTiff(image* map)
{
	NSBitmapImageRep* imageRep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:&map->_data
														 pixelsWide:map->_width
														 pixelsHigh:map->_height
														 bitsPerSample:8
														 samplesPerPixel:4
														 hasAlpha:YES
														 isPlanar:NO
														 colorSpaceName:NSDeviceRGBColorSpace
														 bytesPerRow:4 * map->_width
														 bitsPerPixel:32];
	NSData* result = [imageRep TIFFRepresentationUsingCompression:NSTIFFCompressionLZW factor:0.5];
	[imageRep release];
	return result;
}


static image* ConvertTiffToImage(NSData* data)
{
	NSImage* img = [[NSImage alloc] initWithData:data];
	NSSize size = img.size;
	NSRect rect = NSMakeRect(0, 0, size.width, size.height);
	image* result = new image([img CGImageForProposedRect:&rect context:nil hints:nil]);
	[img release];
	return result;
}


@implementation OpenWarDocument
{
	OpenWarSurface* _surface;
	image* _map;
	NSData* _script;
}


- (id)init
{
    self = [super init];
    if (self)
	{
	}
    return self;
}


- (NSString *)windowNibName
{
    return @"OpenWarDocument";
}


- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
    [super windowControllerDidLoadNib:aController];

}



+ (BOOL)autosavesInPlace
{
    return YES;
}


- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{
	if (_surface != nullptr)
	{
		if ([typeName isEqualToString:@"SmoothMap"])
		{
			_surface->_battleContext->terrainSurfaceModelSmooth->SaveHeightmapToImage();
			return ConvertImageToTiff(_surface->_battleContext->terrainSurfaceModelSmooth->GetMap());
		}
	}

	return nil;
}


- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
	NSLog(@"readFromData");
	if ([typeName isEqualToString:@"SmoothMap"])
	{
		_map = ConvertTiffToImage(data);
	}
	if ([typeName isEqualToString:@"Script"])
	{
		_script = [data retain];
	}

	[self resetSurface];

    return YES;
}


#pragma mark SurfaceFactory


- (Surface*)createSurfaceWithSize:(glm::vec2)size forSurfaceView:(SurfaceView*)surfaceView pixelDensity:(float)pixelDensity;
{
	NSLog(@"createSurfaceWithSize");

	_surface = new OpenWarSurface(size, pixelDensity);
	[self resetSurface];

	return _surface;
}


#pragma mark -

- (void)resetSurface
{
	if (_surface == nullptr)
		return;

	if (_script == nil)
	{
		//NSString* path = @"/Users/nicke/Spikes/jajamensan/BattleScript.lua";
		NSString* path = [[NSBundle mainBundle] pathForResource:@"BattleScript" ofType:@"lua" inDirectory:@"BattleScripts"];
		_script = [[NSData dataWithContentsOfFile:path] retain];
	}

	BattleContext* battleContext = new BattleContext();



	if (battleContext->terrainSurfaceModelSmooth == nullptr && battleContext->terrainSurfaceModelTiled == nullptr)
	{
		if (_map == nullptr)
		{
			NSString* path = [[NSBundle mainBundle] pathForResource:@"DefaultMap" ofType:@"tiff" inDirectory:@"Maps"];
			_map = ConvertTiffToImage([NSData dataWithContentsOfFile:path]);
		}
		battleContext->terrainSurfaceModelSmooth = new TerrainSurfaceModelSmooth(bounds2f(0, 0, 1024, 1024), _map);
	}



	battleContext->terrainFeatureModelBillboard = new TerrainFeatureModelBillboard();
	battleContext->terrainFeatureModelMesh = new TerrainFeatureModelMesh();
	battleContext->billboardTextureAtlas = new BillboardModel();

	BattleScript* battleScript = new BattleScript(battleContext, (const char*)_script.bytes, _script.length);


	if (battleContext->simulationState == nullptr)
	{
		battleContext->simulationState = new SimulationState();
		battleContext->simulationState->terrainModel = static_cast<TerrainSurfaceModel*>(battleContext->terrainSurfaceModelSmooth)
				?: static_cast<TerrainSurfaceModel*>(battleContext->terrainSurfaceModelTiled);
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

	_surface->Reset(battleContext, battleScript);
}




@end
