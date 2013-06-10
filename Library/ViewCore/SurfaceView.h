// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

class Surface;
@protocol SurfaceFactory;


@interface SurfaceView : NSOpenGLView

@property(nonatomic, retain) IBOutlet id<SurfaceFactory> surfaceFactory;

@property(nonatomic, assign) BOOL transparent;

- (Surface*)createSurfaceWithSize:(glm::vec2)size;

@end


@protocol SurfaceFactory <NSObject>

@required

- (Surface*)createSurfaceWithSize:(glm::vec2)size forSurfaceView:(SurfaceView*)surfaceView pixelDensity:(float)pixelDensity;

@optional

- (void)beforeRenderSurface:(Surface*)surface;
- (void)afterRenderSurface:(Surface*)surface;

@end

