/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H


struct renderbuffer
{
	GLuint id;

	renderbuffer(GLenum internalformat, GLsizei width, GLsizei height);
	~renderbuffer();

private:
	renderbuffer(const renderbuffer&) {}
	renderbuffer& operator=(const renderbuffer&) { return *this; }
};


#endif
