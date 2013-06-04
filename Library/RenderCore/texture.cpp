/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "matrix.h"
#include "texture.h"
#include "renderer.h"
#include "image.h"



texture::texture()
{
	glGenTextures(1, &id);
	CHECK_ERROR_GL();
	init();
}



texture::texture(NSString *name)
{
	glGenTextures(1, &id);
	CHECK_ERROR_GL();
	init();
	load(name);
}



texture::texture(const matrix& matrix)
{
	glGenTextures(1, &id);
	CHECK_ERROR_GL();
	init();
	load(matrix);
}


texture::texture(const image& image)
{
	glGenTextures(1, &id);
	CHECK_ERROR_GL();
	init();
	load(image);
}


texture::~texture()
{
	glDeleteTextures(1, &id);
	CHECK_ERROR_GL();
}


void texture::init()
{
	glBindTexture(GL_TEXTURE_2D, id);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_CLAMP_TO_EDGE
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP_TO_EDGE
	CHECK_ERROR_GL();
}



static bool CheckForExtension(NSString *searchName)
{
    NSString *extensionsString = [NSString stringWithCString:(const char*)glGetString(GL_EXTENSIONS) encoding:NSASCIIStringEncoding];
    NSArray *extensionsNames = [extensionsString componentsSeparatedByString:@" "];
    return [extensionsNames containsObject: searchName];
}


void texture::load(NSString *name)
{
#if TARGET_OS_IPHONE
	UIImage* image = nil;

	if ([name hasSuffix:@".png"])
	{
		NSString* stem = [name substringToIndex:name.length - 4];

		if (CheckForExtension(@"GL_IMG_texture_compression_pvrtc"))
		{
			NSString* path = [[NSBundle mainBundle] pathForResource:stem ofType:@"pvrtc" inDirectory:@""];
			NSData* pvrtc = [NSData dataWithContentsOfFile:path];
			if (pvrtc != nil)
			{
				glBindTexture(GL_TEXTURE_2D, id);
				CHECK_ERROR_GL();
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, 1024, 1024, 0, pvrtc.length, pvrtc.bytes);
				CHECK_ERROR_GL();
				return;
			}
		}

		if (renderer_base::pixels_per_point() > 1)
		{
			NSString* name2x = [NSString stringWithFormat:@"%@@2x.png", stem];
			image = [UIImage imageNamed:name2x];
		}
	}

	if (image == nil)
		image = [UIImage imageNamed:name];

	load(image::image(image.CGImage));

#else
	NSImage* image = nil;
	if ([name hasSuffix:@".png"])
	{
		NSString* stem = [name substringToIndex:name.length - 4];
		image = [NSImage imageNamed:stem];
	}

	if (image == nil)
		image = [NSImage imageNamed:name];

	load(image::image([image CGImageForProposedRect:nil context:nil hints:nil]));
#endif
}




static GLubyte to_byte(float value)
{
	if (value < 0)
		return 0;
	if (value > 1)
		return 255;
	return (GLubyte)(255 * value);
}



void texture::load(const matrix& matrix)
{
	glBindTexture(GL_TEXTURE_2D, id);
	CHECK_ERROR_GL();

	int width = matrix._size.n;
	int height = matrix._size.m;
	GLubyte* data = (GLubyte*)calloc((size_t)(width * height * 4), sizeof(GLubyte));
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y)
			data[y * width + x] = to_byte(matrix.operator()(y, x));

	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();

	free(data);
}


void texture::load(const image& image)
{
	glBindTexture(GL_TEXTURE_2D, id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)image._width, (GLsizei)image._height, 0, image._format, GL_UNSIGNED_BYTE, image._data);
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();
}
