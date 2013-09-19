// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef STRING_SHAPE_H
#define STRING_SHAPE_H

#include <map>

#include "../Algebra/bounds.h"

#include "animation.h"
#include "../Graphics/renderer.h"
#include "sprite.h"
#include "../Graphics/vertex.h"
#include "../Graphics/texture.h"


struct string_font
{
	struct item
	{
		wchar_t _character;
        glm::vec2 _bounds_origin;
        glm::vec2 _bounds_size;
		float _u0, _u1;
		float _v0, _v1;
	};

	static image* _image;

#ifndef OPENWAR_SDL
#if TARGET_OS_IPHONE
	UIFont* _font;
#else
	NSFont* _font;
#endif
#endif
    
	renderer<texture_alpha_vertex, string_uniforms>* _renderer;
	float _pixelDensity;
	texture _texture;

	std::map<wchar_t, item> _items;
    glm::vec2 _next;
	bool _dirty;

public:
	string_font(const char* name, float size, float pixelDensity);
	string_font(bool bold, float size, float pixelDensity);
	~string_font();

private:
	void initialize();

public:
	float font_size() const;
	float shadow_offset() const;

	void add_character(wchar_t character);
	item get_character(wchar_t character) const;

	void update_texture();

	glm::vec2 measure(const char* string);
	glm::vec2 get_size(const item& item) const;
};


class string_shape
{
public:
	vertexbuffer<texture_alpha_vertex> _vbo;
	string_font* _font;

	string_shape(string_font* font);

	void clear();
	void add(const char* string, glm::mat4x4 transform, float alpha = 1, float delta = 0);

	void update(GLenum usage);
};



#endif
