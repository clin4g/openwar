// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "image.h"
#include "bounds.h"

#ifdef OPENWAR_SDL
#include <SDL2/SDL_image.h>
#endif
#if TARGET_OS_IPHONE
#include "../Graphics/renderer.h"
#endif


image::image(int width, int height) :
#ifdef OPENWAR_SDL
_surface(nullptr),
#else
_context(nil),
_width(width),
_height(height),
_data(nullptr),
#endif
_format(GL_RGBA)
{
#ifdef OPENWAR_SDL

	_surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

#else

	init_data_context();

#endif
}


#ifdef OPENWAR_SDL
static GLenum sdl_to_gl_pixel_format(Uint32 sdl_pixel_format)
{
	switch (sdl_pixel_format)
	{
		case SDL_PIXELFORMAT_RGB24: return GL_BGR;
		case SDL_PIXELFORMAT_RGB888: return GL_BGRA;

		case SDL_PIXELFORMAT_ABGR8888: return GL_RGBA;
		case SDL_PIXELFORMAT_ARGB8888: return GL_BGRA;
		default: break;
	}

	switch (sdl_pixel_format)
	{
		case SDL_PIXELFORMAT_RGB24: return 0;

		case SDL_PIXELFORMAT_RGBA8888: return 0;
		case SDL_PIXELFORMAT_ABGR8888: return GL_RGBA;
		case SDL_PIXELFORMAT_ARGB8888: return 0;

		case SDL_PIXELFORMAT_RGB888: return 0;

		case SDL_PIXELFORMAT_UNKNOWN: return 0;
		case SDL_PIXELFORMAT_INDEX1LSB: return 0;
		case SDL_PIXELFORMAT_INDEX1MSB: return 0;
		case SDL_PIXELFORMAT_INDEX4LSB: return 0;
		case SDL_PIXELFORMAT_INDEX4MSB: return 0;
		case SDL_PIXELFORMAT_INDEX8: return 0;
		case SDL_PIXELFORMAT_RGB332: return 0;
		case SDL_PIXELFORMAT_RGB444: return 0;
		case SDL_PIXELFORMAT_RGB555: return 0;
		case SDL_PIXELFORMAT_BGR555: return 0;
		case SDL_PIXELFORMAT_ARGB4444: return 0;
		case SDL_PIXELFORMAT_RGBA4444: return 0;
		case SDL_PIXELFORMAT_ABGR4444: return 0;
		case SDL_PIXELFORMAT_BGRA4444: return 0;
		case SDL_PIXELFORMAT_ARGB1555: return 0;
		case SDL_PIXELFORMAT_RGBA5551: return 0;
		case SDL_PIXELFORMAT_ABGR1555: return 0;
		case SDL_PIXELFORMAT_BGRA5551: return 0;
		case SDL_PIXELFORMAT_RGB565: return 0;
		case SDL_PIXELFORMAT_BGR565: return 0;
		case SDL_PIXELFORMAT_BGR24: return 0;
		case SDL_PIXELFORMAT_RGBX8888: return 0;
		case SDL_PIXELFORMAT_BGR888: return 0;
		case SDL_PIXELFORMAT_BGRX8888: return 0;
		case SDL_PIXELFORMAT_BGRA8888: return 0;

		case SDL_PIXELFORMAT_ARGB2101010: return 0;
		case SDL_PIXELFORMAT_YV12: return 0;
		case SDL_PIXELFORMAT_IYUV: return 0;
		case SDL_PIXELFORMAT_YUY2: return 0;
		case SDL_PIXELFORMAT_UYVY: return 0;
		case SDL_PIXELFORMAT_YVYU: return 0;

		default: return 0;
	}
}
#endif


image::image(const resource& r) :
#ifdef OPENWAR_SDL
_surface(nullptr),
#else
_context(nil),
_width(0),
_height(0),
_data(nullptr),
#endif
_format(GL_RGBA)
{
#ifdef OPENWAR_SDL

	const char* s = r.path();
	_surface = IMG_Load(r.path());

	Uint32 format = _surface->format->format;
	if (false) //format == SDL_PIXELFORMAT_RGB888)
	{
		format = SDL_PIXELFORMAT_BGR24;
		_format = GL_RGB;
	}
	else
	{
		format = SDL_PIXELFORMAT_ABGR8888;
		_format = GL_RGBA;
	}

	if (format != _surface->format->format)
	{
		SDL_Surface* surface = SDL_ConvertSurfaceFormat(_surface, format, 0);
		SDL_FreeSurface(_surface);
		_surface = surface;
	}

	_format = sdl_to_gl_pixel_format(_surface->format->format);

#else

	NSString* name = [NSString stringWithFormat:@"%@%@", [NSString stringWithUTF8String:r.name()], [NSString stringWithUTF8String:r.type()]];

#if TARGET_OS_IPHONE
    
	UIImage* img = nil;
	if (renderer_base::pixels_per_point() > 1 && [name hasSuffix:@".png"])
	{
		NSString* stem = [name substringToIndex:name.length - 4];
		NSString* name2x = [NSString stringWithFormat:@"%@@2x.png", stem];
		img = [UIImage imageNamed:name2x];
	}

	if (img == nil)
		img = [UIImage imageNamed:name];

	if (img == nil)
		NSLog(@"image not found: %@", name);

	_format = GL_RGBA;
	_width = CGImageGetWidth(img.CGImage);
	_height = CGImageGetHeight(img.CGImage);

	init_data_context();
    CGContextDrawImage(_context, CGRectMake(0.0f, 0.0f, (CGFloat)_width, (CGFloat)_height), img.CGImage);

#else

	NSImage* image = nil;
	if ([name hasSuffix:@".png"])
	{
		NSString* stem = [name substringToIndex:name.length - 4];
		image = [NSImage imageNamed:stem];
	}

	if (image == nil)
		image = [NSImage imageNamed:name];

	if (image == nil)
		image = [[[NSImage alloc] initWithContentsOfFile:name] autorelease];

	if (image != nil)
	{
		_format = GL_RGBA;
		_width = (int)image.size.width;
		_height = (int)image.size.height;
		init_data_context();
		CGContextDrawImage(_context, CGRectMake(0.0f, 0.0f, (CGFloat) _width, (CGFloat) _height), [image CGImageForProposedRect:nil
																										 context:nil
																										 hints:nil]);
	}

#endif
#endif
}


#ifndef OPENWAR_SDL
image::image(CGImageRef image) :
_context(nil),
_format(GL_RGBA),
_width(CGImageGetWidth(image)),
_height(CGImageGetHeight(image)),
_data(nullptr)
{
	init_data_context();
	CGContextDrawImage(_context, CGRectMake(0.0f, 0.0f, (CGFloat) _width, (CGFloat) _height), image);
}
#endif


image::~image()
{
#ifdef OPENWAR_SDL

#else

	CGContextRelease(_context);
	free(_data);

#endif
}


glm::vec4 image::get_pixel(int x, int y) const
{
	if (0 <= x && x < (int) width() && 0 <= y && y < (int) height())
	{
		const GLubyte* data = reinterpret_cast<const GLubyte*>(pixels());
		const GLubyte* p = data + 4 * (x + width() * y);
		return glm::vec4(p[0], p[1], p[2], p[3]) / 255.0f;
	}
	return glm::vec4();
}


void image::set_pixel(int x, int y, glm::vec4 c)
{
	if (0 <= x && x < (int) width() && 0 <= y && y < (int) height())
	{
		bounds1f bounds(0, 255);
		const GLubyte* data = reinterpret_cast<const GLubyte*>(pixels());
		GLubyte* p = const_cast<GLubyte*>(data) + 4 * (x + width() * y);
		p[0] = (GLubyte)glm::round(bounds.clamp(c.r * 255));
		p[1] = (GLubyte)glm::round(bounds.clamp(c.g * 255));
		p[2] = (GLubyte)glm::round(bounds.clamp(c.b * 255));
		p[3] = (GLubyte)glm::round(bounds.clamp(c.a * 255));
	}
}


void image::premultiply_alpha()
{
	glm::ivec2 s = size();
	for (int x = 0; x < s.x; ++x)
		for (int y = 0; y < s.y; ++y)
		{
			glm::vec4 c = get_pixel(x, y);
			c.r *= c.a;
			c.g *= c.a;
			c.b *= c.a;
			set_pixel(x, y, c);
		}
}





#ifndef OPENWAR_SDL
static CGImageAlphaInfo GetAlphaInfo(GLenum format)
{
	switch (format)
	{
		case GL_RGBA:
		case GL_ALPHA:
		case GL_LUMINANCE_ALPHA:
			return kCGImageAlphaPremultipliedLast;

		default:
			return kCGImageAlphaNone;
	}
}
#endif


#ifndef OPENWAR_SDL
static int count_components(GLenum format)
{
	switch (format)
	{
		case GL_ALPHA:
			return 1;
		case GL_RGB:
			return 3;
		case GL_RGBA:
			return 4;
		case GL_LUMINANCE:
			return 1;
		case GL_LUMINANCE_ALPHA:
			return 2;
		default:
			return 0;
	}
}
#endif


#ifndef OPENWAR_SDL
void image::init_data_context()
{
	int components = count_components(_format);
	_data = (GLubyte*) calloc(_width * _height * components, sizeof(GLubyte));

	CGColorSpaceRef colorSpace = components < 3 ? CGColorSpaceCreateDeviceGray() : CGColorSpaceCreateDeviceRGB();
	_context = CGBitmapContextCreate(_data, _width, _height, 8, _width * components, colorSpace, GetAlphaInfo(_format));
	CGColorSpaceRelease(colorSpace);
}
#endif


#ifndef OPENWAR_SDL
NSData* ConvertImageToTiff(image* map)
{
#if TARGET_OS_IPHONE
	return nil;
#else
	unsigned char* pixels = reinterpret_cast<unsigned char*>(const_cast<GLvoid*>(map->pixels()));
	NSBitmapImageRep* imageRep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:&pixels
														   pixelsWide:map->width()
														   pixelsHigh:map->height()
														   bitsPerSample:8
														   samplesPerPixel:4
														   hasAlpha:YES
														   isPlanar:NO
														   colorSpaceName:NSDeviceRGBColorSpace
														   bytesPerRow:4 * map->width()
														   bitsPerPixel:32];
	NSData* result = [imageRep TIFFRepresentationUsingCompression:NSTIFFCompressionLZW factor:0.5];
	[imageRep release];
	return result;
#endif
}
#endif


#ifndef OPENWAR_SDL
image* ConvertTiffToImage(NSData* data)
{
#if TARGET_OS_IPHONE
	return new image([[UIImage imageWithData:data] CGImage]);
#else
	NSImage* img = [[NSImage alloc] initWithData:data];
	NSSize size = img.size;
	NSRect rect = NSMakeRect(0, 0, size.width, size.height);
	image* result = new image([img CGImageForProposedRect:&rect context:nil hints:nil]);
	[img release];
	return result;
#endif
}
#endif
