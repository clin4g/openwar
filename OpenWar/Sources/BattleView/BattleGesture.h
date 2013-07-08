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
	bool _tappedModiferArea;

	UnitTrackingMarker* _trackingMarker;

	Touch* _trackingTouch;
	Touch* _modifierTouch;

	float _offsetToMarker;
	bool _allowTargetEnemyUnit;

public:
	static bool disableUnitTracking;

	BattleGesture(BattleView* battleView);

	virtual void Update(Surface* surface, double secondsSinceLastUpdate);
	virtual void RenderHints();

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);

	virtual void TouchWasCancelled(Touch* touch);

	//void UpdateTouchMarkers();

private:
	void UpdateTrackingMarker();

	int GetFlipSign() const { return _battleView->GetFlip() ? -1 : 1; }

	Unit* FindFriendlyUnit(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	Unit* FindFriendlyUnitByCurrentPosition(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	Unit* FindFriendlyUnitByFuturePosition(glm::vec2 screenPosition, glm::vec2 terrainPosition);
	Unit* FindFriendlyUnitByModifierArea(glm::vec2 screenPosition, glm::vec2 terrainPosition);

	Unit* FindEnemyUnit(glm::vec2 touchPosition, glm::vec2 markerPosition);

	bounds2f GetUnitCurrentScreenBounds(Unit* unit);
	bounds2f GetUnitFutureScreenBounds(Unit* unit);
	bool IsInsideUnitModifierArea(Unit* unit, glm::vec2 position);
};


#endif
