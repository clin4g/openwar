// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SURFACE_H
#define SURFACE_H

#include "bounds.h"

class Gesture;
class Touch;


class Surface
{
	float _pixelDensity;
	glm::vec2 _origin;
	glm::vec2 _size;

public:
	std::vector<Gesture*> _gestures;

	Surface(glm::vec2 size, float pixelDensity);
	virtual ~Surface();

	float GetPixelDensity() const { return _pixelDensity; }

	glm::vec2 GetOrigin() const { return _origin; }
	void SetOrigin(glm::vec2 value) { _origin = value; }

	glm::vec2 GetSize() const { return _size; }
	void SetSize(glm::vec2 value) { _size = value; }

	void UseViewport();

	bounds2f GetSpriteViewport() const;

	virtual void ScreenSizeChanged();

	virtual void Render() = 0;
	virtual void Update(double secondsSinceLastUpdate) = 0;

	virtual bool ShowContextualMenu(glm::vec2 position);

	virtual void MouseEnter(glm::vec2 position);
	virtual void MouseHover(glm::vec2 position);
	virtual void MouseLeave(glm::vec2 position);
};


#endif
