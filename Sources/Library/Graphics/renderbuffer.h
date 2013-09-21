// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#if TARGET_OS_IPHONE
#include <OpenGLES/ES2/gl.h>
#else
#include <OpenGL/gl.h>
#endif
#else
#if OPENWAR_USE_GLEW
#include <GL/glew.h>
#endif
#include <GL/gl.h>
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
