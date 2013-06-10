// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "animation.h"



std::vector<animation_base**> animation_base::_animations;


void animation_base::update_all(float secondsSinceLastUpdate)
{
	for (auto i = _animations.begin(); i != _animations.end(); ++i)
	{
		animation_base** animation = *i;
		if (*animation != 0)
		{
			(*animation)->update(secondsSinceLastUpdate);
			if ((*animation)->is_done())
			{
				delete *animation;
				*animation = 0;
			}
		}
	}
}


/***/


void animation_float::start()
{
}


void animation_float::update(float secondsSinceLastUpdate)
{
	_animated->_value += _delta * secondsSinceLastUpdate;
}


bool animation_float::is_done()
{
	return false;
}


/***/


void animation_vector2_interpolate_linear::start()
{
	_bounds.min = _animated->_value;
	_t = 0;
}


void animation_vector2_interpolate_linear::update(float secondsSinceLastUpdate)
{
	_t += secondsSinceLastUpdate / _duration;
	if (_t > 1)
		_t = 1;
	_animated->_value = glm::mix(_bounds.min, _bounds.max, _t);
}


bool animation_vector2_interpolate_linear::is_done()
{
	return _t == 1;
}

/***/


void animation_vector2_interpolate_easeinout::start()
{
	_bounds.min = _animated->_value;
	_t = 0;
}


void animation_vector2_interpolate_easeinout::update(float secondsSinceLastUpdate)
{
	_t += secondsSinceLastUpdate / _duration;
	if (_t > 1)
		_t = 1;
	_animated->_value = glm::mix(_bounds.min, _bounds.max, glm::smoothstep(0.0f, 1.0f, _t));
}


bool animation_vector2_interpolate_easeinout::is_done()
{
	return _t == 1;
}
