/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef UNIFORMS_H
#define UNIFORMS_H

#include "matrix.h"
#include "texture.h"


enum shader_uniform_type
{
	shader_uniform_type_int,
	shader_uniform_type_float,
	shader_uniform_type_vector2,
	shader_uniform_type_vector3,
	shader_uniform_type_vector4,
	shader_uniform_type_matrix2,
	shader_uniform_type_matrix3,
	shader_uniform_type_matrix4,
	shader_uniform_type_texture
};

inline shader_uniform_type get_shader_uniform_type(int*) { return shader_uniform_type_int; }
inline shader_uniform_type get_shader_uniform_type(float*) { return shader_uniform_type_float; }
inline shader_uniform_type get_shader_uniform_type(glm::vec2*) { return shader_uniform_type_vector2; }
inline shader_uniform_type get_shader_uniform_type(glm::vec3*) { return shader_uniform_type_vector3; }
inline shader_uniform_type get_shader_uniform_type(glm::vec4*) { return shader_uniform_type_vector4; }
inline shader_uniform_type get_shader_uniform_type(glm::mat2x2*) { return shader_uniform_type_matrix2; }
inline shader_uniform_type get_shader_uniform_type(glm::mat3x3*) { return shader_uniform_type_matrix3; }
inline shader_uniform_type get_shader_uniform_type(glm::mat4x4*) { return shader_uniform_type_matrix4; }
inline shader_uniform_type get_shader_uniform_type(const texture**) { return shader_uniform_type_texture; }


struct plain_uniforms
{
	glm::mat4x4 _transform;
};


struct gradient_uniforms
{
	glm::mat4x4 _transform;
	GLfloat _point_size;

	gradient_uniforms() : _point_size(1) {}
};


struct color_uniforms
{
	glm::mat4x4 _transform;
	GLfloat _point_size;
	glm::vec4 _color;

	color_uniforms() : _point_size(1) {}
};


struct texture_uniforms
{
	glm::mat4x4 _transform;
	const texture* _texture;
};

struct texture_alpha_uniforms
{
	glm::mat4x4 _transform;
	const texture* _texture;
	float _alpha;
};

struct string_uniforms
{
	glm::mat4x4 _transform;
	const texture* _texture;
	glm::vec4 _color;
};


struct ground_uniforms
{
	glm::mat4x4 _transform;
	const texture* _texture;
	glm::vec2 _obstacle1;
	glm::vec2 _obstacle2;
	glm::vec2 _obstacle3;
	glm::vec2 _obstacle4;
	glm::vec2 _obstacle5;
	glm::vec2 _obstacle6;
};


#endif
