// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEGESTURE_H
#define BATTLEGESTURE_H

#include "bounds.h"

#include "Gesture.h"
#include "Touch.h"
#include "BattleModel.h"
#include "BattleView.h"

class UnitTrackingMarker;
class Unit;
class UnitCounter;


class BattleGesture : public Gesture
{
	BattleView* _battleView;

	bool _tappedUnitCenter;
	bool _tappedDestination;

	UnitTrackingMarker* _trackingMarker;

	Touch* _trackingTouch;
	Touch* _modifierTouch;

	float _offsetToMarker;
	bool _allowTargetEnemyUnit;

public:
	static bool disableUnitTracking;

	BattleGesture(BattleView* battleView);

	virtual void Update(double secondsSinceLastUpdate);
	virtual void RenderHints();

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);

	virtual void TouchWasCancelled(Touch* touch);

	//void UpdateTouchMarkers();

private:
	int GetFlipSign() const { return _battleView->GetFlip() ? -1 : 1; }

	Unit* FindNearestTouchUnit(glm::vec2 screenPosition, glm::vec2 terrainPosition);

	bounds2f GetUnitCurrentScreenBounds(Unit* unit);
	bounds2f GetUnitFutureScreenBounds(Unit* unit);

	Unit* GetTouchedUnitMarker(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	Unit* GetTouchedMovementMarker(glm::vec2 screenPosition, glm::vec2 terrainPosition);

	void UpdateTrackingMarker();
	Unit* FindUnit(glm::vec2 touchPosition, glm::vec2 markerPosition, Player player);
};


#endif
