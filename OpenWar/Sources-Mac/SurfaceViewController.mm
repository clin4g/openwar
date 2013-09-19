/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#if TARGET_OS_IPHONE

#import "SurfaceViewController.h"

#include "renderer.h"
#include "Gesture.h"
#include "Touch.h"
#include "Surface.h"



@implementation SurfaceViewController
{
	Surface* _surface;
	EAGLContext* _context;
	std::map<UITouch*, Touch*> _touches;
	std::vector<Touch*> _currentTouches;
	std::set<Gesture*> _currentGestures;
}



- (Surface*)createSurfaceWithSize:(glm::vec2)size
{
	return [_surfaceFactory createSurfaceWithSize:size forSurfaceViewController:self];
}


- (void)dealloc
{
	[_context release];
	[super dealloc];
}


- (void)viewDidLoad
{
	[super viewDidLoad];

	_context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
	if (!_context)
	{
		NSLog(@"Failed to create ES context");
	}

	GLKView* view = (GLKView*)self.view;
	view.context = _context;
	view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

	[EAGLContext setCurrentContext:_context];

	glm::vec2 size = glm::vec2(self.view.bounds.size.width, self.view.bounds.size.height);
	_surface = [self createSurfaceWithSize:size];
}


- (void)viewDidAppear:(BOOL)animated
{
	if (_surface != nullptr)
	{
		_surface->SetSize(glm::vec2(self.view.bounds.size.width, self.view.bounds.size.height));
		_surface->ScreenSizeChanged();
	}
}


- (void)viewDidUnload
{
	[super viewDidUnload];

	if ([EAGLContext currentContext] == _context)
	{
		[EAGLContext setCurrentContext:nil];
	}
	_context = nil;
}


- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];
	// Release any cached data, images, etc. that aren't in use.
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
	{
		return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
	}
	else
	{
		return YES;
	}
}


static bool IsPortrait(UIInterfaceOrientation orientation)
{
	return orientation == UIInterfaceOrientationPortrait || orientation == UIInterfaceOrientationPortraitUpsideDown;
}


- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
		duration:(NSTimeInterval)duration
{
	float width = self.view.bounds.size.width;
	float height = self.view.bounds.size.height;
	if (IsPortrait(self.interfaceOrientation) != IsPortrait(toInterfaceOrientation))
		std::swap(width, height);

	_surface->SetSize(glm::vec2(width, height));
	_surface->ScreenSizeChanged();
}



#pragma mark -



- (void)appendCurrentTouchesAndGestures:(Touch*)touch
{
	if (touch != nullptr)
	{
		_currentTouches.push_back(touch);
		if (touch->GetGesture() != nullptr)
			_currentGestures.insert(touch->GetGesture());
	}
}


- (void)resetCurrentTouchesAndGestures
{
	_currentTouches.clear();
	_currentGestures.clear();

	for (std::pair<UITouch*, Touch*> i : _touches)
		[self appendCurrentTouchesAndGestures:i.second];
}


- (void)resetCurrentTouchesAndGestures:(NSSet*)touches
{
	_currentTouches.clear();
	_currentGestures.clear();

	for (UITouch* original in touches)
		if (original.view == self.view)
			[self appendCurrentTouchesAndGestures:_touches[original]];
}


- (void)createTouch:(UITouch*)original
{
	[original retain];
	glm::vec2 position = [self toVector:[original locationInView:self.view]];
	_touches[original] = new Touch(_surface, original.tapCount, position, original.timestamp, MouseButtons());
}


- (void)updateTouch:(UITouch*)original
{
	Touch* touch = _touches[original];
	if (touch != nullptr)
	{
		glm::vec2 position = [self toVector:[original locationInView:self.view]];
		glm::vec2 previous = [self toVector:[original previousLocationInView:self.view]];
		touch->Update(position, previous, original.timestamp);
	}
}


- (void)deleteTouch:(UITouch*)original
{
	Touch* touch = _touches[original];
	if (touch != nullptr)
	{
		if (touch->GetGesture() != nullptr)
			touch->GetGesture()->UncaptureTouch(touch);
		delete touch;

		_touches.erase(original);
		[original release];
	}
}


- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (_surface == nullptr)
		return;

	for (UITouch* original in touches)
		if (original.view == self.view)
			[self createTouch:original];

	[self resetCurrentTouchesAndGestures:touches];

	for (Touch* touch : _currentTouches)
	{
		if (Gesture::_gestures != nullptr)
			for (Gesture* gesture : *Gesture::_gestures)
				if (gesture->IsEnabled())
					gesture->TouchBegan(touch);
	}
}


- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (_surface == nullptr)
		return;

	for (UITouch* original in touches)
		if (original.view == self.view)
			[self updateTouch:original];

	[self resetCurrentTouchesAndGestures:touches];

	for (Gesture* gesture : _currentGestures)
	{
		gesture->TouchMoved();
	}
}


- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (_surface == nullptr)
		return;

	for (UITouch* original in touches)
		if (original.view == self.view)
			[self updateTouch:original];

	[self touchesEnded:touches cancelled:false];
}


- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (_surface == nullptr)
		return;

	for (UITouch* original in touches)
		if (original.view == self.view)
			[self updateTouch:original];

	[self touchesEnded:touches cancelled:true];
}


- (void)touchesEnded:(NSSet*)touches cancelled:(bool)cancelled
{
	if (_surface == nullptr)
		return;

	[self resetCurrentTouchesAndGestures:touches];

	for (Touch* touch : _currentTouches)
	{
		if (touch->GetGesture() != nullptr)
		{
			if (cancelled)
				touch->GetGesture()->TouchWasCancelled(touch);
			else
				touch->GetGesture()->TouchEnded(touch);
		}
	}

	for (UITouch* original in touches)
		if (original.view == self.view)
			[self deleteTouch:original];
}


- (glm::vec2)toVector:(CGPoint)position
{
	return glm::vec2(position.x, self.view.bounds.size.height - position.y);
}



#pragma mark - GLKViewControllerDelegate


- (void)update
{
	double secondsSinceLastUpdate = self.timeSinceLastUpdate;
	if (_surface != nullptr)
		_surface->Update(secondsSinceLastUpdate);

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->Update(_surface, secondsSinceLastUpdate);

	bool moved = false;
	double timestamp = [NSProcessInfo processInfo].systemUptime;
	for (std::pair<UITouch*, Touch*> i : _touches)
	{
		Touch* touch = i.second;
		double t = touch->GetTimestamp();
		touch->Update(timestamp);
		if (t != touch->GetTimestamp())
			moved = true;
	}

	if (moved)
	{
		[self resetCurrentTouchesAndGestures];

		for (Gesture* gesture : _currentGestures)
		{
			gesture->TouchMoved();
		}
	}
}


#pragma mark - GLKViewDelegate


- (void)glkView:(GLKView*)view drawInRect:(CGRect)rect
{
	if (_surface != nullptr)
	{
		if ([_surfaceFactory respondsToSelector:@selector(beforeRenderSurface:)])
			[_surfaceFactory beforeRenderSurface:_surface];

		_surface->Render();

		if ([_surfaceFactory respondsToSelector:@selector(afterRenderSurface:)])
			[_surfaceFactory afterRenderSurface:_surface];
	}

/*#if GESTURE_RENDER_HINTS
	for (Gesture* gesture : Gesture::_gestures)
		gesture->RenderHints();
#endif*/
}


@end

#endif


