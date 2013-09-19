// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#if !TARGET_OS_IPHONE
#include <GL/glew.h>
#include <GL/gl.h>
//#include <OpenGL/gl.h>
#endif


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
