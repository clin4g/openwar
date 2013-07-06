// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#if !TARGET_OS_IPHONE

#import "SurfaceView.h"

#include "Surface.h"
#include "Gesture.h"
#include "Touch.h"



@implementation SurfaceView
{
	Surface* _surface;
	NSTimer* _timer;
	NSTimeInterval _timestamp;
	BOOL _transparent;
	Touch* _touch;
	bool _mouse;
}



- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format
{
	self = [super initWithFrame:frameRect pixelFormat:format];
	if (self != nil)
	{
		[self setWantsBestResolutionOpenGLSurface:YES];
	}
	return self;
}


- (BOOL)isOpaque
{
	return !_transparent;
}


- (BOOL)isTransparent
{
	return _transparent;
}


- (void)setTransparent:(BOOL)value
{
	_transparent = value;

	GLint opacity = value ? 0 : 1;
	[self.openGLContext setValues:&opacity forParameter:NSOpenGLCPSurfaceOpacity];
}


- (Surface*)createSurfaceWithSize:(glm::vec2)size
{
	[self.openGLContext makeCurrentContext];

	float pixelDensity = (float)[self convertRectToBacking:CGRectMake(0, 0, 1, 1)].size.width;
	Surface* surface = [_surfaceFactory createSurfaceWithSize:size forSurfaceView:self pixelDensity:pixelDensity];

	return surface;
}


- (void)drawRect:(NSRect)dirtyRect
{
	[self.openGLContext makeCurrentContext];

	if (_surface == nullptr)
	{
		if (_surfaceFactory == nil)
			return;

		CGRect bounds = [self convertRect:self.bounds toView:nil];

		_surface = [self createSurfaceWithSize:glm::vec2(bounds.size.width, bounds.size.height)];
		if (_surface == nullptr)
			return;

		_surface->SetOrigin(glm::vec2(bounds.origin.x, bounds.origin.y));

		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glEnable(GL_POINT_SPRITE);

		_timestamp = [NSDate timeIntervalSinceReferenceDate];
		_timer = [NSTimer timerWithTimeInterval:1.0 / 30.0
						  target:self
						  selector:@selector(timerEvent:)
						  userInfo:nil
						  repeats:YES];
		[[NSRunLoop mainRunLoop] addTimer:_timer forMode:NSDefaultRunLoopMode];
	}

	if (_surface == nullptr)
		return;

	if ([_surfaceFactory respondsToSelector:@selector(beforeRenderSurface:)])
		[_surfaceFactory beforeRenderSurface:_surface];

	_surface->Render();
	glFinish();

	if ([_surfaceFactory respondsToSelector:@selector(afterRenderSurface:)])
		[_surfaceFactory afterRenderSurface:_surface];
}


- (void)mouseEnterHoverLeave
{
	if (_surface != nullptr)
	{
		NSPoint p = [self convertPoint:[[self window] convertScreenToBase:[NSEvent mouseLocation]] fromView:nil ];
		bool inside = CGRectContainsPoint([self bounds], p);

		if (inside)
		{
			if (!_mouse)
			{
				_surface->MouseEnter([self toVector:p]);
				_mouse = true;
			}
			else
			{
				_surface->MouseHover([self toVector:p]);
			}
		}
		else if (_mouse)
		{
			_surface->MouseLeave([self toVector:p]);
			_mouse = false;
		}
	}
}


- (void)timerEvent:(NSTimer*)t
{
	if (_surface != nullptr)
	{
		[self mouseEnterHoverLeave];

		[self.openGLContext makeCurrentContext];

		NSTimeInterval timestamp = t.fireDate.timeIntervalSinceReferenceDate;
		double secondsSinceLastUpdate = timestamp - _timestamp;

		_surface->Update(secondsSinceLastUpdate);
		if (Gesture::_gestures != nullptr)
			for (Gesture* gesture : *Gesture::_gestures)
				gesture->Update(_surface, secondsSinceLastUpdate);

		_timestamp = timestamp;

		if (_touch != nullptr)
		{
			double oldTimestamp = _touch->GetTimestamp();
			_touch->Update([NSProcessInfo processInfo].systemUptime);

			if (_touch->GetTimestamp() != oldTimestamp && _touch->GetGesture() != nullptr)
				_touch->GetGesture()->TouchMoved();
		}

		[self setNeedsDisplay:YES];
	}
}


- (void)update
{
	static bool _update = false;
	if (_update)
		return;

	_update = true;

	[super update];

	if (_surface != nullptr)
	{
		[self.openGLContext makeCurrentContext];
		CGRect bounds = [self convertRect:self.bounds toView:nil];
		_surface->SetOrigin(glm::vec2(bounds.origin.x, bounds.origin.y));
		_surface->SetSize(glm::vec2(bounds.size.width, bounds.size.height));
		_surface->ScreenSizeChanged();
	}

	_update = false;
}


- (void)reshape
{
	[super reshape];

	if (_surface != nullptr)
	{
		[self.openGLContext makeCurrentContext];
		CGRect bounds = [self convertRect:self.bounds toView:nil];
		_surface->SetOrigin(glm::vec2(bounds.origin.x, bounds.origin.y));
		_surface->SetSize(glm::vec2(bounds.size.width, bounds.size.height));
		_surface->ScreenSizeChanged();
	}
}


#pragma mark -


static MouseButtons CurrentMouseButtons()
{
	NSUInteger buttons = [NSEvent pressedMouseButtons];
	bool left = (buttons & 1) != 0;
	bool right = (buttons & 2) != 0;
	bool other = (buttons & 3) != buttons;
	return MouseButtons(left, right, other);
}


- (void)dispatchMouseEvent:(NSEvent*)event
{
	[self.openGLContext makeCurrentContext];

	if (_touch == nullptr)
	{
		glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];
		_touch = new Touch(_surface, (int)event.clickCount, position, event.timestamp, CurrentMouseButtons());

		if (Gesture::_gestures != nullptr)
			for (Gesture* gesture : *Gesture::_gestures)
				if (gesture->IsEnabled())
					gesture->TouchBegan(_touch);

		[self acceptsFirstResponder];
	}
	else
	{
		glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];
		_touch->Update(position, event.timestamp, CurrentMouseButtons());

		if (_touch->GetGesture() != nullptr && _touch->GetCurrentButtons().Any())
			_touch->GetGesture()->TouchMoved();
	}

	if (!_touch->GetCurrentButtons().Any())
	{
		if (_touch->GetGesture() != nullptr)
			_touch->GetGesture()->TouchEnded(_touch);

		delete _touch;
		_touch = nullptr;
	}

	[self setNeedsDisplay:true];
}


- (BOOL)mouseDownCanMoveWindow
{
	//return _transparent;
	return NO;
}


- (BOOL)acceptsFirstMouse:(NSEvent*)event
{
	//return !_transparent;
	return YES;
}


- (void)mouseDown:(NSEvent*)event
{
	[self dispatchMouseEvent:event];
}


- (void)rightMouseDown:(NSEvent*)event
{
	if ([self menuForEvent:event] != nil)
		[super rightMouseDown:event];
	else
		[self dispatchMouseEvent:event];
}


- (void)otherMouseDown:(NSEvent*)event
{
	[self dispatchMouseEvent:event];
}


- (void)mouseDragged:(NSEvent*)event
{
	[self dispatchMouseEvent:event];
}


- (void)rightMouseDragged:(NSEvent*)event
{
	[self dispatchMouseEvent:event];
}


- (void)otherMouseDragged:(NSEvent*)event
{
	[self dispatchMouseEvent:event];
}


- (void)mouseUp:(NSEvent*)event
{
	[self dispatchMouseEvent:event];
}


- (void)rightMouseUp:(NSEvent*)event
{
	[self dispatchMouseEvent:event];
}


- (void)otherMouseUp:(NSEvent*)event
{
	[self dispatchMouseEvent:event];
}


- (void)scrollWheel:(NSEvent*)event
{
	glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];

	float k = [event hasPreciseScrollingDeltas] ? 1 : 3;

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			if (gesture->IsEnabled())
				gesture->ScrollWheel(_surface, position, k * glm::vec2(event.deltaX, event.deltaY));
}


- (void)magnifyWithEvent:(NSEvent*)event
{
	glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			if (gesture->IsEnabled())
				gesture->Magnify(_surface, position, (float)event.magnification);
}


- (void)smartMagnifyWithEvent:(NSEvent*)event
{
	glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			if (gesture->IsEnabled())
				gesture->Magnify(_surface, position);
}


- (NSMenu*)menuForEvent:(NSEvent*)event
{
	if (event.type == NSRightMouseDown && _surface != nullptr)
	{
		glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];
		if (!_surface->ShowContextualMenu(position))
			return nil;
	}

	return [super menuForEvent:event];
}


- (BOOL)acceptsFirstResponder
{
	return YES;
}


static char TranslateKeyCode(unsigned short keyCode)
{
	switch (keyCode)
	{
		case 12: return 'Q';
		case 13: return 'W';
		case 14: return 'E';
		case 0: return 'A';
		case 1: return 'S';
		case 2: return 'D';
		case 18: return '1';
		case 19: return '2';
		case 20: return '3';
		case 21: return '4';
		case 23: return '5';
		case 22: return '6';
		case 26: return '7';
		default: return '\0';
	}
}


- (void)keyDown:(NSEvent*)event
{
	char key = TranslateKeyCode(event.keyCode);
	if (key == '\0')
		return;

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			if (gesture->IsEnabled())
				gesture->KeyDown(_surface, key);
}


- (void)keyUp:(NSEvent*)event
{
	char key = TranslateKeyCode(event.keyCode);
	if (key == '\0')
		return;

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			if (gesture->IsEnabled())
				gesture->KeyUp(_surface, key);
}


#pragma mark -



- (glm::vec2)toVector:(CGPoint)position
{
	return glm::vec2(position.x, position.y);
}


@end

#endif
