// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef IMAGE_H
#define IMAGE_H


class image
{
public:
	GLenum _format;
	size_t _width;
	size_t _height;
	GLubyte* _data;
	CGContextRef _context;

	image(size_t width, size_t height, GLenum format = GL_RGBA);
	image(const char* name);
	image(CGImageRef image);
	~image();

	glm::ivec2 size() const { return glm::ivec2(_width, _height); }

	glm::vec4 get_pixel(int x, int y) const;
	void set_pixel(int x, int y, glm::vec4 c);

private:
	void init_data_context();
};


NSData* ConvertImageToTiff(image* map);
image* ConvertTiffToImage(NSData* data);


#endif
