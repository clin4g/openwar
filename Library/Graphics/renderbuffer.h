// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <OpenGL/gl.h>


struct renderbuffer
{
	GLuint id;

	renderbuffer(GLenum internalformat, GLsizei width, GLsizei height);
	~renderbuffer();

	void resize(GLenum internalformat, GLsizei width, GLsizei height);

private:
	renderbuffer(const renderbuffer&) {}
	renderbuffer& operator=(const renderbuffer&) { return *this; }
};


#endif
