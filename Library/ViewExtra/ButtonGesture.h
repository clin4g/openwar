/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef ButtonGesture_H
#define ButtonGesture_H

#include "Gesture.h"

class ButtonItem;
class ButtonView;


class ButtonGesture : public Gesture
{
	ButtonItem* _buttonItem;

public:
	std::vector<ButtonView*> buttonViews;

	ButtonGesture();
	virtual ~ButtonGesture();

	virtual void Update(double secondsSinceLastUpdate);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);
};


#endif
