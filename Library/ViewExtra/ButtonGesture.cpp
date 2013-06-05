/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "ButtonGesture.h"
#include "ButtonView.h"
#include "Touch.h"



ButtonGesture::ButtonGesture() :
_buttonItem(nullptr)
{
}



ButtonGesture::~ButtonGesture()
{

}



void ButtonGesture::Update(double secondsSinceLastUpdate)
{

}



void ButtonGesture::TouchBegan(Touch* touch)
{
	if (touch->HasGesture() || !_touches.empty())
		return;

	for (ButtonView* buttonView : buttonViews)
		if (buttonView->GetScreen() == touch->GetSurface())
			for (ButtonArea* buttonArea : buttonView->GetButtonAreas())
				for (ButtonItem* buttonItem : buttonArea->buttonItems)
					if (buttonItem->HasAction()
							&& !buttonItem->IsDisabled()
							&& buttonItem->GetBounds().contains(touch->GetPosition()))
					{
						_buttonItem = buttonItem;
					}

	if (_buttonItem != nullptr)
	{
		CaptureTouch(touch);
		_buttonItem->SetHighlight(true);
	}
	else
	{
		for (ButtonView* buttonView : buttonViews)
			if (buttonView->GetScreen() == touch->GetSurface())
				for (ButtonArea* buttonArea : buttonView->GetButtonAreas())
				{
					buttonArea->noaction();
				}
	}
}



void ButtonGesture::TouchMoved()
{
	if (_buttonItem != nullptr && !_touches.empty())
	{
		_buttonItem->SetHighlight(_buttonItem->GetBounds().contains(_touches.front()->GetPosition()));
	}
}



void ButtonGesture::TouchEnded(Touch* touch)
{
	if (_buttonItem != nullptr)
	{
		if (_buttonItem->GetBounds().contains(touch->GetPosition()))
			_buttonItem->CallAction();

		_buttonItem->SetHighlight(false);
		_buttonItem = nullptr;
	}
}
