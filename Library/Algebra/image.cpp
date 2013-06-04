/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "image.h"
#include "bounds.h"


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


image::image(size_t width, size_t height, GLenum format) :
_format(format),
_width(width),
_height(height),
_data(nullptr),
_context(nil)
{
	init_data_context();

}


image::image(NSString* name) :
_format(GL_RGBA),
_width(0),
_height(0),
_data(nullptr),
_context(nil)
{
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
}


image::image(CGImageRef image) :
_format(GL_RGBA),
_width(CGImageGetWidth(image)),
_height(CGImageGetHeight(image)),
_data(nullptr),
_context(nil)
{
	init_data_context();
	CGContextDrawImage(_context, CGRectMake(0.0f, 0.0f, (CGFloat) _width, (CGFloat) _height), image);
}


image::~image()
{
	free(_data);
	CGContextRelease(_context);
}


glm::vec4 image::get_pixel(int x, int y) const
{
	if (0 <= x && x < (int) _width && 0 <= y && y < (int) _height)
	{
		GLubyte* p = _data + 4 * (x + _width * y);
		return glm::vec4(p[0], p[1], p[2], p[3]) / 255.0f;
	}
	return glm::vec4();
}


void image::set_pixel(int x, int y, glm::vec4 c)
{
	if (0 <= x && x < (int) _width && 0 <= y && y < (int) _height)
	{
		bounds1f bounds(0, 255);
		GLubyte* p = _data + 4 * (x + _width * y);
		p[0] = (GLubyte) bounds.clamp(c.r * 255);
		p[1] = (GLubyte) bounds.clamp(c.g * 255);
		p[2] = (GLubyte) bounds.clamp(c.b * 255);
		p[3] = (GLubyte) bounds.clamp(c.a * 255);
	}
}


void image::init_data_context()
{
	int components = count_components(_format);

	_data = (GLubyte*) calloc(_width * _height * components, sizeof(GLubyte));

	CGColorSpaceRef colorSpace = components < 3 ? CGColorSpaceCreateDeviceGray() : CGColorSpaceCreateDeviceRGB();
	_context = CGBitmapContextCreate(_data, _width, _height, 8, _width * components, colorSpace, GetAlphaInfo(_format));
	CGColorSpaceRelease(colorSpace);
}
