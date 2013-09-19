// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

#include "../Algebra/bounds.h"


template <class T> class animated;
template <class T> class animation_sequence;

class animation_base
{
public:
	static std::vector<animation_base**> _animations;

public:
	virtual ~animation_base() {}

	static void update_all(float secondsSinceLastUpdate);

	virtual void start() = 0;
	virtual void update(float secondsSinceLastUpdate) = 0;
	virtual bool is_done() = 0;
};


template <class T>
class animation : public animation_base
{
public:
	animated<T>* _animated;
	animation() : _animated(0) { }
};




template <class T>
class animated
{
	animation_base* _animation;

public:
	T _value;

	animated(const T& v) :  _animation(0), _value(v)
	{
		animation_base::_animations.push_back(&_animation);
	}


	animated() : _animation(0), _value()
	{
		animation_base::_animations.push_back(&_animation);
	}

	~animated()
	{
		auto i = std::find(animation_base::_animations.begin(), animation_base::_animations.end(), &_animation);
		if (i != animation_base::_animations.end())
			animation_base::_animations.erase(i);
		delete _animation;
	}

	bool is_animating() const
	{
		return _animation != 0;
	}

	animated<T>& operator=(const T& v)
	{
		delete _animation;
		_animation = 0;
		_value = v;
		return *this;
	}

	animated<T>& operator=(animation<T>* animation)
	{
		delete _animation;
		_animation = 0;

		animation->_animated = this;
		_animation = animation;
		_animation->start();

		return *this;
	}

	animation_sequence<T>* sequence(int count = 0)
	{
		animation_sequence<T>* sequence = new animation_sequence<T>(count);
		*this = sequence;
		return sequence;
	}

};


template <class T>
class animation_pause : public animation<T>
{
	float _duration;
	float _t;
public:
	animation_pause(float duration) : _duration(duration), _t(0) {}
	virtual void start()
	{
		_t = 0;
	}
	virtual void update(float secondsSinceLastUpdate)
	{
		_t += secondsSinceLastUpdate;
	}
	virtual bool is_done()
	{
		return _t >= _duration;
	}
};


template <class T>
class animation_assign : public animation<T>
{
	T _value;
public:
	animation_assign(T value) : _value(value) {}
	virtual void start()
	{
		this->_animated->_value = _value;
	}
	virtual void update(float secondsSinceLastUpdate)
	{
	}
	virtual bool is_done()
	{
		return true;
	}
};


template <class T>
class animation_sequence : public animation<T>
{
	std::vector<animation<T>*> _animations;
	int _index;
	int _iterationCount;
	int _iterationIndex;
public:
	animation_sequence(int count = 0): _index(0), _iterationCount(count), _iterationIndex(0) {}

	virtual ~animation_sequence()
	{
		for (auto i = _animations.begin(); i != _animations.end(); ++i)
		{
			animation<T>* a = *i;
			delete a;
		}
	}


	animation_sequence* add(animation<T>* animation)
	{
		animation->_animated = this->_animated;
		_animations.push_back(animation);
		return this;
	}

	animation_sequence* pause(float duration)
	{
		add(new animation_pause<T>(duration));
		return this;
	}

	animation_sequence* assign(T value)
	{
		add(new animation_assign<T>(value));
		return this;
	}


	virtual void start()
	{
		for (auto i = _animations.begin(); i != _animations.end(); ++i)
		{
			animation<T>* animation = *i;
			animation->_animated = this->_animated;
		}
		_index = -1;
		_iterationIndex = 0;
		next_animation();
	}

	void next_animation()
	{
		while (true)
		{
			++_index;
			if (_index == _animations.size())
			{
				if (_animations.size() == 0)
					break;
				if (_iterationCount != 0 && ++_iterationIndex == _iterationCount)
					break;
				_index = 0;
			}

			_animations[_index]->start();
			if (!_animations[_index]->is_done())
				break;
		}
	}

	virtual void update(float secondsSinceLastUpdate)
	{
		if (_index < _animations.size() && _animations[_index]->is_done())
			next_animation();
		if (_index < _animations.size())
			_animations[_index]->update(secondsSinceLastUpdate);
	}
	virtual bool is_done()
	{
		return _index == _animations.size();
	}
};



class animation_float : public animation<float>
{
	float _delta;
public:
	animation_float(float delta) : _delta(delta) {}
	virtual void start();
	virtual void update(float secondsSinceLastUpdate);
	virtual bool is_done();
};


class animation_vector2_interpolate_linear : public animation<glm::vec2>
{
	float _duration;
	bounds2f _bounds;
	float _t;
public:
	animation_vector2_interpolate_linear(float duration, glm::vec2 target) : _duration(duration), _bounds(target, target), _t(0) {}
	virtual void start();
	virtual void update(float secondsSinceLastUpdate);
	virtual bool is_done();
};


class animation_vector2_interpolate_easeinout : public animation<glm::vec2>
{
	float _duration;
	bounds2f _bounds;
	float _t;
public:
	animation_vector2_interpolate_easeinout(float duration, glm::vec2 target) : _duration(duration), _bounds(target, target), _t(0) {}
	virtual void start();
	virtual void update(float secondsSinceLastUpdate);
	virtual bool is_done();
};


#endif
