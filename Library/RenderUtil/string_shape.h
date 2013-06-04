/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef STRING_SHAPE_H
#define STRING_SHAPE_H

#include "bounds.h"
#include "matrix.h"

#include "animation.h"
#include "renderer.h"
#include "sprite.h"
#include "vertex.h"
#include "texture.h"


struct string_font
{
	struct item
	{
		unichar _character;
		CGRect _bounds;
		float _u0, _u1;
		float _v0, _v1;
	};

	static image* _image;

#if TARGET_OS_IPHONE
	UIFont* _font;
#else
	NSFont* _font;
#endif

	renderer<texture_alpha_vertex, string_uniforms>* _renderer;
	float _pixelDensity;
	texture _texture;

	std::map<unichar, item> _items;
	CGPoint _next;
	bool _dirty;

public:
	string_font(NSString* name, CGFloat size, float pixelDensity);
	string_font(bool bold, CGFloat size, float pixelDensity);
	~string_font();

private:
	void initialize();

public:
	float font_size() const;
	float shadow_offset() const;

	void add_character(unichar character);
	item get_character(unichar character) const;

	void update_texture();

	glm::vec2 measure(NSString* string);
	glm::vec2 get_size(const item& item) const;
};


class string_shape : public shape<texture_alpha_vertex>
{
public:
	string_font* _font;

	string_shape(string_font* font);

	void clear();
	void add(NSString* string, glm::mat4x4 transform, float alpha = 1, float delta = 0);

	virtual void update(GLenum usage);
};



#endif
