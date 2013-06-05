/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef RENDERER_H
#define RENDERER_H

#include "vertexbuffer.h"
#include "uniforms.h"

#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif


inline GLint get_vertex_attribute_size(float*) { return 1; }
inline GLenum get_vertex_attribute_type(float*) { return GL_FLOAT; }

inline GLint get_vertex_attribute_size(glm::vec2*) { return 2; }
inline GLenum get_vertex_attribute_type(glm::vec2*) { return GL_FLOAT; }

inline GLint get_vertex_attribute_size(glm::vec3*) { return 3; }
inline GLenum get_vertex_attribute_type(glm::vec3*) { return GL_FLOAT; }

inline GLint get_vertex_attribute_size(glm::vec4*) { return 4; }
inline GLenum get_vertex_attribute_type(glm::vec4*) { return GL_FLOAT; }


struct renderer_vertex_attribute
{
	const GLchar* _name;
	GLint _size;
	GLenum _type;
	GLsizei _stride;
	GLintptr _offset;

	renderer_vertex_attribute(const GLchar* name, GLint size, GLenum type, GLsizei stride, GLintptr offset)
		: _name(name), _size(size), _type(type), _stride(stride), _offset(offset)
	{
	}
};


struct renderer_shader_uniform
{
	GLint _location;
	const GLchar* _name;
	shader_uniform_type _type;
	int _offset;
	GLenum _texture;

	renderer_shader_uniform(const GLchar* name, shader_uniform_type type, int offset)
		: _location(0), _name(name), _type(type), _offset(offset)
	{
		_texture = 0;
	}

	void set_value(const void* uniforms);
};


struct renderer_vertex_shader
{
	const GLchar* _source;

	renderer_vertex_shader(const GLchar* source) : _source(source) { }
};


struct renderer_fragment_shader
{
	const GLchar* _source;
	renderer_fragment_shader(const GLchar* source) : _source(source) { }
};


struct renderer_specification
{
	std::vector<renderer_vertex_attribute> _vertex_attributes;
	std::vector<renderer_shader_uniform> _shader_uniforms;
	const char* _vertex_shader;
	const char* _fragment_shader;
	renderer_specification() : _vertex_shader(nullptr), _fragment_shader(nullptr) { }
};


renderer_specification operator,(renderer_vertex_attribute x, renderer_vertex_attribute y);
renderer_specification operator,(renderer_vertex_attribute x, renderer_shader_uniform y);
renderer_specification operator,(renderer_vertex_attribute x, renderer_vertex_shader y);
renderer_specification operator,(renderer_specification x, renderer_vertex_attribute y);
renderer_specification operator,(renderer_specification x, renderer_shader_uniform y);
renderer_specification operator,(renderer_specification x, renderer_vertex_shader y);
renderer_specification operator,(renderer_specification x, renderer_fragment_shader y);


#define MEMBER_POINTER(_Vertex, _Name) &((_Vertex*)nullptr)->_Name

#define VERTEX_ATTRIBUTE_SIZE(_Vertex, _Name) get_vertex_attribute_size(MEMBER_POINTER(_Vertex, _Name))
#define VERTEX_ATTRIBUTE_TYPE(_Vertex, _Name) get_vertex_attribute_type(MEMBER_POINTER(_Vertex, _Name))
#define VERTEX_ATTRIBUTE_STRIDE(_Vertex, _Name) sizeof(_Vertex)
#define VERTEX_ATTRIBUTE_OFFSET(_Vertex, _Name) (GLintptr)MEMBER_POINTER(_Vertex, _Name)

#define VERTEX_ATTRIBUTE(_Vertex, _Name) \
	renderer_vertex_attribute(#_Name, \
		VERTEX_ATTRIBUTE_SIZE(_Vertex, _Name), \
		VERTEX_ATTRIBUTE_TYPE(_Vertex, _Name), \
		VERTEX_ATTRIBUTE_STRIDE(_Vertex, _Name), \
		VERTEX_ATTRIBUTE_OFFSET(_Vertex, _Name))

#define SHADER_UNIFORM_TYPE(_Uniforms, _Name) get_shader_uniform_type(MEMBER_POINTER(_Uniforms, _Name))
#define SHADER_UNIFORM_OFFSET(_Uniforms, _Name) (const char*)&((_Uniforms*)nullptr)->_Name - (const char*)nullptr


#define SHADER_UNIFORM(_Uniforms, _Name) \
	renderer_shader_uniform(#_Name, \
		SHADER_UNIFORM_TYPE(_Uniforms, _Name), \
		SHADER_UNIFORM_OFFSET(_Uniforms, _Name))


#define VERTEX_SHADER(source) renderer_vertex_shader(#source)
#define FRAGMENT_SHADER(source) renderer_fragment_shader(#source)



class renderer_base
{
public:
	std::vector<renderer_vertex_attribute> _vertex_attributes;
	std::vector<renderer_shader_uniform> _shader_uniforms;
	GLuint _program;
	GLenum _blend_sfactor;
	GLenum _blend_dfactor;

	renderer_base(const renderer_specification& specification);
	virtual ~renderer_base();

	static float pixels_per_point();

	static GLuint compile_shader(GLenum type, const char* source);

	static bool link_program(GLuint program);
	static bool validate_program(GLuint program);
};



template <class _Vertex, class _Uniforms>
class renderer : public renderer_base
{
public:
	typedef _Vertex vertex_type;
	typedef _Uniforms uniforms_type;

	renderer(const renderer_specification& specification) : renderer_base(specification)
	{
	}

	void render(shape<vertex_type>& shape, const uniforms_type& uniforms)
	{
		if (shape._vertices.empty())
			return;

		glUseProgram(_program);
		CHECK_ERROR_GL();

		shape.bind(_vertex_attributes);

		for (int i = 0; i < (int)_shader_uniforms.size(); ++i)
		{
			_shader_uniforms[i].set_value(&uniforms);
		}

		if (_blend_sfactor != GL_ONE || _blend_dfactor != GL_ZERO)
		{
			glEnable(GL_BLEND);
			CHECK_ERROR_GL();
			glBlendFunc(_blend_sfactor, _blend_dfactor);
			CHECK_ERROR_GL();
		}

		glDrawArrays(shape._mode, 0, shape.count());
		CHECK_ERROR_GL();

		if (_blend_sfactor != GL_ONE || _blend_dfactor != GL_ZERO)
		{
			glDisable(GL_BLEND);
			CHECK_ERROR_GL();
			glBlendFunc(GL_ONE, GL_ZERO);
			CHECK_ERROR_GL();
		}

		shape.unbind(_vertex_attributes);
	}

};


typedef renderer<plain_vertex, color_uniforms> plain_renderer;
typedef renderer<color_vertex, gradient_uniforms> gradient_renderer;
typedef renderer<texture_vertex, texture_uniforms> texture_renderer;


struct renderers
{
	static renderers* singleton;

	renderer<texture_vertex, texture_uniforms>* _distance_renderer;
	renderer<color_vertex, gradient_uniforms>* _gradient_renderer;
	renderer<color_vertex3, gradient_uniforms>* _gradient_renderer3;
	renderer<texture_vertex, ground_uniforms>* _ground_renderer;
	renderer<plain_vertex, color_uniforms>* _plain_renderer;
	renderer<texture_vertex, texture_uniforms>* _texture_renderer;
	renderer<texture_vertex, texture_uniforms>* _opaque_texture_renderer;
	renderer<texture_vertex, texture_alpha_uniforms>* _alpha_texture_renderer;

	renderers();
};


#endif
