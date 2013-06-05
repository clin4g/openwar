/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#import "Document.h"
#include "OpenWarSurface.h"


@implementation Document
{
	OpenWarSurface* _surface;
	NSImage* _image;
}


- (id)init
{
    self = [super init];
    if (self)
	{
		NSString* mapDir = @"/Users/nicke/Projects/Samurai/Media/Maps Textures";
		NSString* mapName = @"Map1";
		NSString* name = [NSString stringWithFormat:@"%@/%@-Forest.png", mapDir, mapName];
		_image = [[NSImage alloc] initWithContentsOfFile:name];
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
	return [_image TIFFRepresentation];
}


- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
	_image = [[NSImage alloc] initWithData:data];

    return YES;
}



#pragma mark SurfaceFactory


- (Surface*)createSurfaceWithSize:(glm::vec2)size forSurfaceView:(SurfaceView*)surfaceView pixelDensity:(float)pixelDensity;
{
	_surface = new OpenWarSurface(size, pixelDensity);
	return _surface;
}


@end
