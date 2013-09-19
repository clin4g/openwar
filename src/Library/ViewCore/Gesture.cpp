// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Gesture.h"
#include "Touch.h"
#include "Surface.h"


std::vector<Gesture*>* Gesture::_gestures = nullptr;


Gesture::Gesture() :
_enabled(true)
{
	if (_gestures == nullptr)
		_gestures = new std::vector<Gesture*>();
	_gestures->push_back(this);
}


Gesture::~Gesture()
{
	auto i = std::find(_gestures->begin(), _gestures->end(), this);
	if (i != _gestures->end())
		_gestures->erase(i);
}


/*void Gesture::Update(double secondsSinceLastUpdate)
{
}*/



void Gesture::RenderHints()
{
}


void Gesture::KeyDown(Surface* surface, char key)
{
}


void Gesture::KeyUp(Surface* surface, char key)
{
}


void Gesture::ScrollWheel(Surface* surface, glm::vec2 position, glm::vec2 delta)
{
}


void Gesture::Magnify(Surface* surface, glm::vec2 position, float magnification)
{
}


void Gesture::Magnify(Surface* surface, glm::vec2 position)
{
}


void Gesture::TouchWasCaptured(Touch* touch)
{
}


void Gesture::TouchWasUncaptured(Touch* touch)
{
}


void Gesture::TouchWasCancelled(Touch* touch)
{
}


void Gesture::CaptureTouch(Touch* touch)
{
	_touches.push_back(touch);
	touch->_gesture = this;
}


void Gesture::UncaptureTouch(Touch* touch)
{
	auto i = std::find(_touches.begin(), _touches.end(), touch);
	if (i != _touches.end())
		_touches.erase(i);

	if (touch->_gesture == this)
	{
		touch->_gesture = nullptr;
	}
}
