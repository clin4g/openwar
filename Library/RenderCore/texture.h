/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef TEXTURE_H
#define TEXTURE_H

class matrix;
class image;


struct texture
{
	GLuint id;

	texture();
	texture(NSString *name);
	texture(const image& image);
	texture(const matrix& matrix);
	~texture();

	void init();

	void load(NSString *name);
	void load(const image& image);
	void load(const matrix& matrix);

private:
	texture(const texture&) {}
	texture& operator=(const texture&) { return *this; }
};


#endif
