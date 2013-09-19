// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>

#include "SmoothTerrainSky.h"


SmoothTerrainSky::SmoothTerrainSky()
{
	_textureBackgroundLinen = new texture(resource("Linen128x128.png"));
}


SmoothTerrainSky::~SmoothTerrainSky()
{
}


void SmoothTerrainSky::Render(renderers* _renderers, float cameraDirectionZ, bool flip)
{
	vertexbuffer<color_vertex> shape;

	float y = cameraDirectionZ;
	float x = sqrtf(1 - y * y);
	float a = 1 - fabsf(atan2f(y, x) / (float)M_PI_2);
	float blend = bounds1f(0, 1).clamp(3.0f * (a - 0.3f));

	shape._mode = GL_TRIANGLES;
	shape._vertices.clear();

	glm::vec4 c1 = glm::vec4(56, 165, 230, 0) / 255.0f;
	glm::vec4 c2 = glm::vec4(160, 207, 243, 255) / 255.0f;
	c2.a = blend;

	shape._vertices.push_back(color_vertex(glm::vec2(-1, 0.2), c1));
	shape._vertices.push_back(color_vertex(glm::vec2(-1, 1.0), c2));
	shape._vertices.push_back(color_vertex(glm::vec2(1, 1.0), c2));
	shape._vertices.push_back(color_vertex(glm::vec2(1, 1.0), c2));
	shape._vertices.push_back(color_vertex(glm::vec2(1, 0.2), c1));
	shape._vertices.push_back(color_vertex(glm::vec2(-1, 0.2), c1));

	gradient_uniforms uniforms;
	uniforms._transform = flip ? glm::scale(glm::mat4x4(), glm::vec3(-1.0f, -1.0f, 1.0f)) : glm::mat4x4();

	_renderers->_gradient_renderer->render(shape, uniforms);
}



void SmoothTerrainSky::RenderBackgroundLinen(renderers* _renderers, bounds2f viewport, bool flip)
{
	vertexbuffer<texture_vertex> shape;

	shape._mode = GL_TRIANGLES;
	shape._vertices.clear();

	glm::vec2 vt0 = glm::vec2();
	glm::vec2 vt1 = viewport.size() / 128.0f;

	shape._vertices.push_back(texture_vertex(glm::vec2(-1, -1), glm::vec2(vt0.x, vt0.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(-1, 1), glm::vec2(vt0.x, vt1.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(1, 1), glm::vec2(vt1.x, vt1.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(1, 1), glm::vec2(vt1.x, vt1.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(1, -1), glm::vec2(vt1.x, vt0.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(-1, -1), glm::vec2(vt0.x, vt0.y)));

	texture_uniforms uniforms;
	uniforms._transform = flip ? glm::scale(glm::mat4x4(), glm::vec3(-1.0f, -1.0f, 1.0f)) : glm::mat4x4();
	uniforms._texture = _textureBackgroundLinen;

	_renderers->_texture_renderer->render(shape, uniforms);
}
