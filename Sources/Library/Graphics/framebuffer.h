// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

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
#ifdef OPENWAR_USE_GLES2
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif
#endif

#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif

struct renderbuffer;
struct texture;


struct framebuffer
{
	GLuint id;

	framebuffer();
	~framebuffer();

	void attach_color(renderbuffer* value);
	void attach_color(texture* value);

	void attach_depth(renderbuffer* value);
	void attach_depth(texture* value);

	void attach_stencil(renderbuffer* value);

private:
	framebuffer(const framebuffer&) {}
	framebuffer& operator = (const framebuffer&) {return *this;}
};


class bind_framebuffer
{
	GLint _old;

public:
	bind_framebuffer(const framebuffer& fb)
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_old);
		glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
		CHECK_ERROR_GL();
	}

	~bind_framebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _old);
	}
};


#endif
