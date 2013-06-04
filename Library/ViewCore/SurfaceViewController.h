/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

class Surface;
@protocol SurfaceFactory;


@interface SurfaceViewController : GLKViewController

@property(nonatomic, retain) IBOutlet id <SurfaceFactory> surfaceFactory;

- (Surface*)createSurfaceWithSize:(glm::vec2)size;

- (void)update;

@end


@protocol SurfaceFactory <NSObject>

@required

- (Surface*)createSurfaceWithSize:(glm::vec2)size
		forSurfaceViewController:(SurfaceViewController*)surfaceViewController;

@optional

- (void)beforeRenderSurface:(Surface*)surface;
- (void)afterRenderSurface:(Surface*)surface;

@end

