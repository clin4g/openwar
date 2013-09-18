// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef IMAGE_H
#define IMAGE_H

#ifdef OPENWAR_SDL
#endif

#include "resource.h"


class image
{
#ifdef OPENWAR_SDL
	SDL_Surface* _surface;
#else
	CGContextRef _context;
	size_t _width;
	size_t _height;
	GLubyte* _data;
#endif
	GLenum _format;

public:
	image(int width, int height);
	image(const resource& r);

#ifndef OPENWAR_SDL
	image(CGImageRef image);
	CGContextRef CGContext() const { return _context; }
#endif

	~image();

#ifdef OPENWAR_SDL
	GLsizei width() const { return (GLsizei)_surface->w; }
	GLsizei height() const { return (GLsizei)_surface->h; }
	GLenum format() const { return _format; }
	const GLvoid* pixels() const { return _surface->pixels; }
	glm::ivec2 size() const { return glm::ivec2(_surface->w, _surface->h); }
#else
	GLsizei width() const { return (GLsizei)_width; }
	GLsizei height() const { return (GLsizei)_height; }
	GLenum format() const { return _format; }
	const GLvoid* pixels() const { return _data; }
	glm::ivec2 size() const { return glm::ivec2(_width, _height); }
#endif

	glm::vec4 get_pixel(int x, int y) const;
	void set_pixel(int x, int y, glm::vec4 c);

	void premultiply_alpha();

private:
	void init_data_context();
};


#ifndef OPENWAR_SDL
NSData* ConvertImageToTiff(image* map);
image* ConvertTiffToImage(NSData* data);
#endif


#endif
