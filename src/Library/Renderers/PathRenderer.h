// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef PathRenderer_H
#define PathRenderer_H

#include <functional>
#include <vector>

class GradientTriangleRenderer;


class PathRenderer
{
	std::function<glm::vec3(glm::vec2)> _getPosition;
	glm::vec4 _color;
	float _offset;

public:
	PathRenderer(std::function<glm::vec3(glm::vec2)> getPosition);
	~PathRenderer();

	glm::vec4 GetColor() const { return _color; }
	void SetColor(glm::vec4 value) { _color = value; }

	float GetOffset() const { return _offset; }
	void SetOffset(float value) { _offset = value; }

	void Path(GradientTriangleRenderer* renderer, const std::vector<glm::vec2>& path, int mode);

private:
	void RenderPath(GradientTriangleRenderer* renderer, const std::vector<glm::vec2>& path);
};



#endif
