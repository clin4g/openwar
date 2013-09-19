// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TOUCH_H
#define TOUCH_H

#include "sampler.h"

class Gesture;
class Surface;


struct MouseButtons
{
	bool left, right, other;
	MouseButtons(bool l = false, bool r = false, bool o = false) : left(l), right(r), other(o) { }
	bool Any() const { return left || right || other; }
};

inline bool operator==(MouseButtons v1, MouseButtons v2) { return v1.left == v2.left && v1.right == v2.right && v1.other == v2.other; }
inline bool operator!=(MouseButtons v1, MouseButtons v2) { return v1.left != v2.left || v1.right != v2.right || v1.other != v2.other; }


enum class Motion { Unknown, Stationary, Moving };


class Touch
{
	friend class Gesture;
	Surface* _surface;
	Gesture* _gesture;
	int _tapCount;
	bool _hasMoved;
	glm::vec2 _position;
	glm::vec2 _previous;
	glm::vec2 _original;
	double _timestart;
	double _timestamp;
	sampler _sampler;
	MouseButtons _currentButtons;
	MouseButtons _previousButtons;

public:
	Touch(Surface* surface, int tapCount, glm::vec2 position, double timestamp, MouseButtons buttons);
	~Touch();

	int GetTapCount() const { return _tapCount; }

	Surface* GetSurface() const { return _surface; }
	Gesture* GetGesture() const { return _gesture; }
	bool HasGesture() const { return _gesture != nullptr; }

	void Update(glm::vec2 position, glm::vec2 previous, double timestamp);
	void Update(glm::vec2 position, double timestamp, MouseButtons buttons);
	void Update(double timestamp);

	glm::vec2 GetPosition() const { return _position; }
	glm::vec2 GetPrevious() const { return _previous; }
	glm::vec2 GetOriginal() const { return _original; }

	double GetTimeStart() const { return _timestart; }
	double GetTimestamp() const { return _timestamp; }

	MouseButtons GetCurrentButtons() const { return _currentButtons; }
	MouseButtons GetPreviousButtons() const { return _previousButtons; }

	Motion GetMotion() const;

	bool HasMoved() const { return _hasMoved; }
	void ResetHasMoved();

	void ResetVelocity();
	glm::vec2 GetVelocity() const { return GetVelocity(_timestamp); }
	glm::vec2 GetVelocity(double timestamp) const;

};


#endif
