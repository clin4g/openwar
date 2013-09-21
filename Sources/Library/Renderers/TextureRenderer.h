// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TextureRenderer_H
#define TextureRenderer_H

#include "../Graphics/renderer.h"


class TextureRenderer
{
protected:
	struct vertex
	{
		glm::vec3 _position;
		glm::vec2 _texcoord;

		vertex() {}
		vertex(glm::vec3 p, glm::vec2 t) : _position(p), _texcoord(t) {}
	};

	struct uniforms
	{
		glm::mat4x4 _transform;
		const texture* _texture;
	};

	renderer<vertex, uniforms>* _renderer;
	vertexbuffer<vertex> _vbo;

public:
	TextureRenderer();
	virtual ~TextureRenderer();

	virtual void Reset() = 0;
	void Draw(const glm::mat4x4& transform, const texture* texture);
};


class TextureTriangleRenderer : public TextureRenderer
{
public:
	virtual ~TextureTriangleRenderer();
	virtual void Reset();

	void AddVertex(glm::vec3 p, glm::vec2 t);
};


#endif
