/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "sprite.h"
#include "renderer.h"



sprite_base::sprite_base() :
_visible(true)
{
}



sprite_base::~sprite_base()
{
}



bounds2f* sprite_transform::_default_viewport = nullptr;



sprite_transform::sprite_transform() :
_viewport(-1, -1, 1, 1),
_translate(glm::vec2(0, 0)),
_scale(glm::vec2(1, 1)),
_rotate(0)
{
	if (_default_viewport != nullptr)
		_viewport = *_default_viewport;
}



sprite_transform::sprite_transform(bounds2f viewport) :
_viewport(viewport),
_translate(glm::vec2(0, 0)),
_scale(glm::vec2(1, 1)),
_rotate(0)
{
}



sprite_transform::sprite_transform(bounds2f viewport, glm::vec2 translate) :
_viewport(viewport),
_translate(translate),
_scale(glm::vec2(1, 1)),
_rotate(0)
{
}



sprite_transform::~sprite_transform()
{
}



glm::mat4x4 sprite_transform::transform() const
{
	glm::vec2 viewport_center = _viewport.center();
	glm::vec2 viewport_scale = 2.0f / _viewport.size();

	glm::mat4x4 result = glm::scale(glm::mat4x4(), glm::vec3(viewport_scale.x, viewport_scale.y, 1.0f))
			* glm::translate(glm::mat4x4(), glm::vec3(_translate._value.x - viewport_center.x, _translate._value.y - viewport_center.y, 0.0f));

	result = result * glm::scale(glm::mat4x4(), glm::vec3(_scale._value.x, _scale._value.y, 1.0f));

	if (_rotate._value != 0)
		result = result * glm::rotate(glm::mat4x4(), _rotate._value * 180 / (float)M_PI, glm::vec3(0, 0, 1));

	return result;
}



gradient_sprite::gradient_sprite(renderer<color_vertex, gradient_uniforms>* renderer) : sprite<color_vertex, gradient_uniforms>(renderer)
{
}



plain_sprite::plain_sprite(renderer<plain_vertex, color_uniforms>* renderer) : sprite<plain_vertex, color_uniforms>(renderer)
{
}



string_sprite::string_sprite(renderer<texture_alpha_vertex, string_uniforms>* renderer) : sprite<texture_alpha_vertex, string_uniforms>(renderer),
	_texture(nullptr),
	_color(glm::vec4(1, 1, 1, 1))
{
}



texture_sprite::texture_sprite(renderer<texture_vertex, texture_uniforms>* renderer) : texture_sprite_base<texture_vertex>(renderer)
{
}
