// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "framebuffer.h"
#include "renderbuffer.h"
#include "texture.h"


framebuffer::framebuffer()
{
	glGenFramebuffers(1, &id);
	CHECK_ERROR_GL();
}


framebuffer::~framebuffer()
{
	glDeleteFramebuffers(1, &id);
	CHECK_ERROR_GL();
}


void framebuffer::attach_color(renderbuffer* value)
{
	bind_framebuffer binding(*this);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, value->id);
	CHECK_ERROR_GL();
}


void framebuffer::attach_color(texture* value)
{
	bind_framebuffer binding(*this);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, value->id, 0);
	CHECK_ERROR_GL();
}


void framebuffer::attach_depth(renderbuffer* value)
{
	bind_framebuffer binding(*this);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, value->id);
	CHECK_ERROR_GL();
}


void framebuffer::attach_depth(texture* value)
{
	bind_framebuffer binding(*this);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, value->id, 0);
	CHECK_ERROR_GL();
}


void framebuffer::attach_stencil(renderbuffer* value)
{
	bind_framebuffer binding(*this);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, value->id);
	CHECK_ERROR_GL();
}
