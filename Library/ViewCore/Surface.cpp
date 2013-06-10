// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Surface.h"

#include "renderer.h"
#include "Gesture.h"



Surface::Surface(glm::vec2 size, float pixelDensity) :
_pixelDensity(pixelDensity),
_size(size)
{
}


Surface::~Surface()
{
}


void Surface::UseViewport()
{
	GLsizei width = (GLsizei)(_size.x * _pixelDensity);
	GLsizei height = (GLsizei)(_size.y * _pixelDensity);
	glViewport(0, 0, width, height);
}


bounds2f Surface::GetSpriteViewport() const
{
	return bounds2f(0, 0, _size);
}


void Surface::ScreenSizeChanged()
{

}


/*void Surface::Update(double secondsSinceLastUpdate)
{
}*/



bool Surface::ShowContextualMenu(glm::vec2 position)
{
	return false;
}


void Surface::MouseEnter(glm::vec2 position)
{
}


void Surface::MouseHover(glm::vec2 position)
{
}


void Surface::MouseLeave(glm::vec2 position)
{
}
