// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#import "Document.h"
#include "OpenWarSurface.h"
#include "SimulationState.h"
#include "BattleContext.h"


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


static SimulationState* LoadSimulationState(image* map)
{
	SimulationState* result = new SimulationState();

	if (map == nullptr)
	{
		NSString* path = [[NSBundle mainBundle] pathForResource:@"DefaultMap" ofType:@"tiff" inDirectory:@"Maps"];
		map = ConvertTiffToImage([NSData dataWithContentsOfFile:path]);
	}

	result->smoothTerrainModel = new SmoothTerrainModel(bounds2f(0, 0, 1024, 1024), map);

	return result;
}


@implementation Document
{
	OpenWarSurface* _surface;
	image* _map;
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
    return @"Document";
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
		_surface->_battleContext->smoothTerrainModel->SaveHeightmapToImage();
		return ConvertImageToTiff(_surface->_battleContext->smoothTerrainModel->GetMap());
	}

	return nil;
}


- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
	if ([typeName isEqualToString:@"SmoothMap"])
	{
		_map = ConvertTiffToImage(data);
	}

	if (_surface != nullptr)
		_surface->Reset(LoadSimulationState(_map));

    return YES;
}


#pragma mark SurfaceFactory


- (Surface*)createSurfaceWithSize:(glm::vec2)size forSurfaceView:(SurfaceView*)surfaceView pixelDensity:(float)pixelDensity;
{
	_surface = new OpenWarSurface(size, pixelDensity);
	_surface->Reset(LoadSimulationState(_map));
	return _surface;
}


@end
