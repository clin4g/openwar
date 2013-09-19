// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SPRITE_H
#define SPRITE_H

#include "animation.h"
#include "../Algebra/bounds.h"

#include "../Graphics/renderer.h"


struct sprite_base
{
	animated<bool> _visible;

	sprite_base();
	virtual ~sprite_base();
	virtual void render() const = 0;
};


struct sprite_transform
{
	static bounds2f* _default_viewport;
	bounds2f _viewport;
	animated<glm::vec2> _translate;
	animated<glm::vec2> _scale;
	animated<float> _rotate;

	sprite_transform();
	sprite_transform(bounds2f viewport);
	sprite_transform(bounds2f viewport, glm::vec2 translate);
	virtual ~sprite_transform();

	glm::mat4x4 transform() const;
};


template <class _Vertex, class _Uniforms>
struct sprite : public sprite_base
{
	typedef _Vertex vertex_type;
	typedef _Uniforms uniforms_type;
	typedef vertexbuffer<vertex_type> shape_type;
	typedef renderer<vertex_type, uniforms_type> renderer_type;

	renderer_type* _renderer;
	animated<shape_type*> _shape;

	sprite(renderer_type* renderer) : _renderer(renderer), _shape(nullptr) {}

	virtual void render() const
	{
		if (_visible._value && _renderer != nullptr && _shape._value != nullptr)
		{
			_renderer->render(*_shape._value, uniforms());
		}
	}

	virtual uniforms_type uniforms() const = 0;
};



struct gradient_sprite : public sprite<color_vertex, gradient_uniforms>, public sprite_transform
{
	gradient_sprite(renderer<color_vertex, gradient_uniforms>* renderer);

	virtual gradient_uniforms uniforms() const
	{
		gradient_uniforms result;
		result._transform = transform();
		return result;
	}
};



struct plain_sprite : public sprite<plain_vertex, color_uniforms>, public sprite_transform
{
	animated<glm::vec4> _color;

	plain_sprite(renderer<plain_vertex, color_uniforms>* renderer);

	virtual uniforms_type uniforms() const
	{
		plain_sprite::uniforms_type result;
		result._transform = transform();
		result._color = _color._value;
		return result;
	}
};



struct string_sprite : public sprite<texture_alpha_vertex, string_uniforms>, public sprite_transform
{
	animated<texture*> _texture;
	animated<glm::vec4> _color;

	string_sprite(renderer<texture_alpha_vertex, string_uniforms>* renderer);

	virtual string_uniforms uniforms() const
	{
		string_uniforms result;
		result._transform = transform();
		result._texture = _texture._value;
		result._color = _color._value;
		return result;
	}
};



template <class _Vertex>
struct texture_sprite_base : public sprite<_Vertex, texture_uniforms>, public sprite_transform
{
	animated<texture*> _texture;

	texture_sprite_base(renderer<_Vertex, texture_uniforms>* renderer) : sprite<_Vertex, texture_uniforms>(renderer),
		_texture(nullptr)
	{
	}

	virtual texture_uniforms uniforms() const
	{
		texture_uniforms result;
		result._transform = transform();
		result._texture = _texture._value;
		return result;
	}
};



struct texture_sprite : public texture_sprite_base<texture_vertex>
{
	texture_sprite(renderer<texture_vertex, texture_uniforms>* renderer);
};


typedef texture_sprite texture_texture_sprite;



#endif
