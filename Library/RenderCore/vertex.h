// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef VERTEX_H
#define VERTEX_H


#if !TARGET_OS_IPHONE
#define glGenVertexArraysOES glGenVertexArraysAPPLE
#define glBindVertexArrayOES glBindVertexArrayAPPLE
#define glDeleteVertexArraysOES glDeleteVertexArraysAPPLE
#endif



struct plain_vertex
{
	glm::vec2 _position;

	plain_vertex() {}
	plain_vertex(glm::vec2 p) : _position(p) {}
};


struct plain_vertex3
{
	glm::vec3 _position;

	plain_vertex3() {}
	plain_vertex3(glm::vec3 p) : _position(p) {}
};


struct color_vertex
{
	glm::vec2 _position;
	glm::vec4 _color;

	color_vertex() {}
	color_vertex(glm::vec2 p, glm::vec4 c) : _position(p), _color(c) {}
	color_vertex(float x, float y, float r, float g, float b, float a) : _position(x, y), _color(r, g, b, a) {}
};


struct color_vertex3
{
	glm::vec3 _position;
	glm::vec4 _color;

	color_vertex3() {}
	color_vertex3(glm::vec3 p, glm::vec4 c) : _position(p), _color(c) {}
};


struct texture_vertex
{
	glm::vec2 _position;
	glm::vec2 _texcoord;

	texture_vertex() {}
	texture_vertex(glm::vec2 p, glm::vec2 t) : _position(p), _texcoord(t) {}
	texture_vertex(float px, float py, float tx, float ty) : _position(px, py), _texcoord(tx, ty) {}
};


struct texture_vertex3
{
	glm::vec3 _position;
	glm::vec2 _texcoord;

	texture_vertex3() {}
	texture_vertex3(glm::vec3 p, glm::vec2 t) : _position(p), _texcoord(t) {}
};


struct texture_alpha_vertex
{
	glm::vec2 _position;
	glm::vec2 _texcoord;
	float _alpha;

	texture_alpha_vertex() {}
	texture_alpha_vertex(glm::vec2 p, glm::vec2 t, float a) : _position(p), _texcoord(t), _alpha(a) {}
};


#endif

