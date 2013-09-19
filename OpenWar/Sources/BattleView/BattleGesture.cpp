// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleGesture.h"
#include "BattleModel.h"
#include "BattleView.h"
#include "SoundPlayer.h"
#include "UnitCounter.h"
#include "UnitTrackingMarker.h"
#include "UnitMovementMarker.h"

#include "sprite.h"
#include "PlainRenderer.h"


#define SNAP_TO_UNIT_TRESHOLD 22 // meters
#define KEEP_ORIENTATION_TRESHHOLD 40 // meters
#define MODIFIER_AREA_RADIUS_MIN 5.0f // meters
#define MODIFIER_AREA_RADIUS_MAX 25.0f // meters


bool BattleGesture::disableUnitTracking = false;


BattleGesture::BattleGesture(BattleView* battleView) :
_battleView(battleView),
_trackingMarker(0),
_trackingTouch(0),
_modifierTouch(0),
_tappedUnitCenter(false),
_tappedDestination(false),
_tappedModiferArea(false),
_offsetToMarker(0),
_allowTargetEnemyUnit(false)
{
}


void BattleGesture::Update(Surface* surface, double secondsSinceLastUpdate)
{

}


void BattleGesture::RenderHints()
{
	_battleView->UseViewport();

	vertexbuffer<plain_vertex> shape;
	shape._mode = GL_LINES;

	plain_sprite sprite(renderers::singleton->_plain_renderer);
	sprite._shape = &shape;
	sprite._color = glm::vec4(0, 0, 0, 0.2f);
	sprite._viewport = _battleView->GetViewportBounds();

	for (UnitCounter* unitMarker : _battleView->GetBattleModel()->_unitMarkers)
	{
		bounds2f bounds = GetUnitCurrentBounds(unitMarker->_unit);

		shape._vertices.push_back(plain_vertex(bounds.p11()));
		shape._vertices.push_back(plain_vertex(bounds.p12()));
		shape._vertices.push_back(plain_vertex(bounds.p12()));
		shape._vertices.push_back(plain_vertex(bounds.p22()));
		shape._vertices.push_back(plain_vertex(bounds.p22()));
		shape._vertices.push_back(plain_vertex(bounds.p21()));
		shape._vertices.push_back(plain_vertex(bounds.p21()));
		shape._vertices.push_back(plain_vertex(bounds.p11()));

		bounds = GetUnitFutureBounds(unitMarker->_unit);
		if (!bounds.is_empty())
		{
			shape._vertices.push_back(plain_vertex(bounds.p11()));
			shape._vertices.push_back(plain_vertex(bounds.p12()));
			shape._vertices.push_back(plain_vertex(bounds.p12()));
			shape._vertices.push_back(plain_vertex(bounds.p22()));
			shape._vertices.push_back(plain_vertex(bounds.p22()));
			shape._vertices.push_back(plain_vertex(bounds.p21()));
			shape._vertices.push_back(plain_vertex(bounds.p21()));
			shape._vertices.push_back(plain_vertex(bounds.p11()));
		}

		bounds = GetUnitModifierBounds(unitMarker->_unit);
		if (!bounds.is_empty())
		{
			shape._vertices.push_back(plain_vertex(bounds.p11()));
			shape._vertices.push_back(plain_vertex(bounds.p12()));
			shape._vertices.push_back(plain_vertex(bounds.p12()));
			shape._vertices.push_back(plain_vertex(bounds.p22()));
			shape._vertices.push_back(plain_vertex(bounds.p22()));
			shape._vertices.push_back(plain_vertex(bounds.p21()));
			shape._vertices.push_back(plain_vertex(bounds.p21()));
			shape._vertices.push_back(plain_vertex(bounds.p11()));
		}
	}

	sprite.render();


	/*PlainLineRenderer renderer;
	renderer.Reset();
	for (UnitCounter* unitMarker : _battleView->GetBattleModel()->_unitMarkers)
	{
		glm::vec2 center = !unitMarker->_unit->command.path.empty() ? unitMarker->_unit->command.path.back() : unitMarker->_unit->state.center;
		float facing = unitMarker->_unit->command.facing;

		glm::vec2 p1 = center + MODIFIER_AREA_RADIUS_MAX * vector2_from_angle(facing - 0.5f * glm::half_pi<float>());
		glm::vec2 p2 = center + MODIFIER_AREA_RADIUS_MAX * vector2_from_angle(facing + 0.5f * glm::half_pi<float>());

		renderer.AddLine(_battleView->GetPosition(center), _battleView->GetPosition(p1));
		renderer.AddLine(_battleView->GetPosition(center), _battleView->GetPosition(p2));

		for (int i = 1; i <= 10; ++i)
		{
			float a1 = facing + ((i - 1) / 10.0f - 0.5f) * glm::half_pi<float>();
			float a2 = facing + (i / 10.0f - 0.5f) * glm::half_pi<float>();

			p1 = center + MODIFIER_AREA_RADIUS_MIN * vector2_from_angle(a1);
			p2 = center + MODIFIER_AREA_RADIUS_MIN * vector2_from_angle(a2);
			renderer.AddLine(_battleView->GetPosition(p1), _battleView->GetPosition(p2));

			p1 = center + MODIFIER_AREA_RADIUS_MAX * vector2_from_angle(a1);
			p2 = center + MODIFIER_AREA_RADIUS_MAX * vector2_from_angle(a2);
			renderer.AddLine(_battleView->GetPosition(p1), _battleView->GetPosition(p2));
		}
	}
	renderer.Draw(_battleView->GetTransform(), glm::vec4(0, 0, 0, 0.2f));*/
}


void BattleGesture::TouchBegan(Touch* touch)
{
	if (touch->GetSurface() != _battleView->GetSurface())
		return;
	if (touch->GetGesture() != nullptr)
		return;
	if (!_battleView->GetViewportBounds().contains(touch->GetPosition()))
		return;

	glm::vec2 screenPosition = touch->GetPosition();
	glm::vec2 terrainPosition = _battleView->GetTerrainPosition3(screenPosition).xy();
	Unit* unit = FindFriendlyUnit(screenPosition, terrainPosition);

	if (_trackingTouch == nullptr)
	{
		if (unit == nullptr)
			return;

		if (unit != nullptr && _battleView->GetTrackingMarker(unit) == nullptr)
		{
			_allowTargetEnemyUnit = unit->stats.unitWeapon == UnitWeaponBow || unit->stats.unitWeapon == UnitWeaponArq;
			_trackingMarker = _battleView->AddTrackingMarker(unit);

			float distanceToUnitCenter = glm::distance(GetUnitCurrentBounds(unit).center(), screenPosition);
			float distanceToDestination = glm::distance(GetUnitFutureBounds(unit).center(), screenPosition);
			float distanceToModifierArea = glm::distance(GetUnitModifierBounds(unit).center(), screenPosition);
			float distanceMinimum = glm::min(distanceToUnitCenter, glm::min(distanceToDestination, distanceToModifierArea));

			_tappedUnitCenter = distanceToUnitCenter == distanceMinimum;
			_tappedDestination = distanceToDestination == distanceMinimum && !_tappedUnitCenter;
			_tappedModiferArea = distanceToModifierArea == distanceMinimum && !_tappedUnitCenter && !_tappedDestination;

			if (_tappedDestination || _tappedModiferArea)
			{
				_offsetToMarker = 0;//(_boardView->ContentToScreen(vector3(unit->movement.GetFinalDestination(), 0)).y - _boardView->ContentToScreen(vector3(terrainPosition, 0)).y) * GetFlipSign();
				if (_offsetToMarker < 0)
					_offsetToMarker = 0;

				std::vector<glm::vec2>& path = _trackingMarker->_path;
				path.clear();
				path.insert(path.begin(), unit->command.path.begin(), unit->command.path.end());

				//_trackingMarker->SetDestination(&unit->command.waypoint);

				glm::vec2 orientation = unit->command.GetDestination() + 18.0f * vector2_from_angle(unit->command.facing);
				_trackingMarker->SetOrientation(&orientation);
			}
			else
			{
				_offsetToMarker = 0;//(_boardView->ContentToScreen(vector3(unit->state.center, 0)).y - _boardView->ContentToScreen(vector3(terrainPosition, 0)).y) * GetFlipSign();
				if (_offsetToMarker < 0)
					_offsetToMarker = 0;

				glm::vec2 orientation = unit->state.center + 18.0f * vector2_from_angle(unit->state.direction);
				_trackingMarker->SetOrientation(&orientation);
			}

			if (touch->GetTapCount() > 1 && _tappedUnitCenter && !_tappedDestination)
			{
				unit->command.meleeTarget = nullptr;
				unit->command.ClearPathAndSetDestination(unit->state.center);
				unit->command.missileTarget = nullptr;
				unit->command.missileTargetLocked = false;
			}

			_trackingMarker->SetRunning(touch->GetTapCount() > 1 || (!_tappedUnitCenter && unit->command.running));

			CaptureTouch(touch);
			_trackingTouch = touch;
		}
		else if (_modifierTouch == nullptr)
		{
			CaptureTouch(touch);
			_modifierTouch = touch;
		}
		else
		{
			CaptureTouch(touch);
			_trackingTouch = touch;
		}
	}
	else if (_modifierTouch == nullptr)
	{
		if (unit != nullptr)
			return;

		if (_gestures != nullptr)
		{
			for (Gesture* g : *_gestures)
			{
				BattleGesture* gesture = dynamic_cast<BattleGesture*>(g);
				if (gesture != nullptr && gesture != this && gesture->_trackingTouch != nullptr)
				{
					if (glm::length(_trackingTouch->GetPosition() - touch->GetPosition()) > glm::length(gesture->_trackingTouch->GetPosition() - touch->GetPosition()))
						return;
				}
			}
		}

		CaptureTouch(touch);
		_modifierTouch = touch;
	}
}


void BattleGesture::TouchMoved()
{
	if (_trackingMarker != nullptr)
	{
#if  TARGET_OS_IPHONE
		static int* _icon_size = nullptr;
		if (_icon_size == nullptr)
			_icon_size = new int([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone ? 57 : 72);

		glm::vec2 oldPosition = _trackingTouch->GetPrevious();//_boardView->GetTerrainPosition(_trackingTouch->_previous).xy();
		glm::vec2 newPosition = _trackingTouch->GetPosition();//_boardView->GetTerrainPosition(_trackingTouch->_position).xy();

		float diff = (newPosition.y - oldPosition.y) * GetFlipSign();
		if (diff < 0)
			_offsetToMarker -= diff / 2;
		else
			_offsetToMarker += diff;

		if (_offsetToMarker > *_icon_size / 2)
			_offsetToMarker = *_icon_size / 2;
#endif

		if (_trackingTouch->GetCurrentButtons() != _trackingTouch->GetPreviousButtons())
			_trackingTouch->ResetHasMoved();

		if (_trackingTouch->HasMoved())
		{
			UpdateTrackingMarker();
		}
	}
}


void BattleGesture::TouchEnded(Touch* touch)
{
	if (touch == _trackingTouch)
	{
		if (_trackingMarker != nullptr)
		{
			Unit* unit = _trackingMarker->GetUnit();
			unit->command.path.clear();

			std::vector<glm::vec2>& path = _trackingMarker->_path;
			if (!path.empty())
			{
				unit->command.path.insert(unit->command.path.begin(), path.begin(), path.end());
			}

			unit->command.meleeTarget = _trackingMarker->GetMeleeTarget();
			unit->command.running = _trackingMarker->GetRunning();

			/*if (destinationUnit != nullptr)
			{
				unit->command.waypoint = destinationUnit->state.center;
			}
			else if (destination)
			{
				unit->command.waypoint = *destination;
			}*/

			Unit* orientationUnit = _trackingMarker->GetMissileTarget();
			glm::vec2* orientation = _trackingMarker->GetOrientationX();

			if (unit->command.holdFire)
			{
				unit->command.missileTarget = nullptr;
				unit->command.missileTargetLocked = false;
				unit->state.loadingTimer = 0;
			}
			else if (orientationUnit != nullptr)
			{
				unit->command.missileTarget = orientationUnit;
				unit->command.missileTargetLocked = true;
				unit->command.facing = angle(orientationUnit->state.center - unit->command.GetDestination());
				unit->state.loadingTimer = 0;
			}
			else if (orientation)
			{
				unit->command.facing = angle(*orientation - unit->command.GetDestination());
			}

			if (!touch->HasMoved())
			{
				if (_tappedUnitCenter && touch->GetTapCount() > 1)
				{
					unit->command.meleeTarget = nullptr;
					unit->command.ClearPathAndSetDestination(unit->state.center);
					unit->command.missileTarget = nullptr;
					unit->command.missileTargetLocked = false;
				}
				else if (_tappedDestination && !_tappedUnitCenter)
				{
					if (!unit->command.running)
					{
						unit->command.running = true;
					}
				}
				else if (_tappedUnitCenter && !_tappedDestination)
				{
					if (unit->command.running)
					{
						unit->command.running = false;
					}
				}
			}

			unit->timeUntilSwapFighters = 0.2f;

			if (_battleView->GetMovementMarker(unit) == nullptr)
				_battleView->AddMovementMarker(unit);

			if (touch->GetTapCount() == 1)
				SoundPlayer::singleton->Play(SoundBufferCommandAck);

			_battleView->RemoveTrackingMarker(_trackingMarker);
			_trackingMarker = nullptr;
		}
	}


	if (touch == _modifierTouch && _trackingTouch != nullptr)
	{
		_trackingTouch->ResetHasMoved();
	}


	if (_trackingTouch != nullptr && _modifierTouch != nullptr)
	{
		_trackingTouch->ResetVelocity();
		_modifierTouch->ResetVelocity();
	}


	/*if (_trackingTouch == nullptr && _modifierTouch != nullptr)
	{
		if (!ViewState::singleton->showTitleScreen)
		{
			vector2 velocity = touch->GetVelocity();
			float speed = norm(velocity) - 1;
			if (speed < 0)
				speed = 0;
			else if (speed > 10)
				speed = 10;
			velocity = vector2::from_polar(velocity.angle(), speed);

			float k = SamuraiSurface::singleton->renderLayer.actualWidth / 2;
			if (SamuraiSurface::singleton->renderLayer.flip)
				k = -k;

			BoardGesture::scrollVelocity = velocity * k;
		}
	}*/


	if (touch == _trackingTouch)
	{
		_trackingTouch = nullptr;
	}
	else if (touch == _modifierTouch)
	{
		_modifierTouch = nullptr;
	}
}


void BattleGesture::TouchWasCancelled(Touch* touch)
{
	if (_trackingMarker)
	{
		_battleView->RemoveTrackingMarker(_trackingMarker);
		_trackingMarker = nullptr;
	}

}


/***/



void BattleGesture::UpdateTrackingMarker()
{
	Unit* unit = _trackingMarker->GetUnit();

	glm::vec2 screenTouchPosition = _trackingTouch->GetPosition();
	glm::vec2 screenMarkerPosition = screenTouchPosition + glm::vec2(0, 1) * (_offsetToMarker * GetFlipSign());
	glm::vec2 touchPosition = _battleView->GetTerrainPosition3(screenTouchPosition).xy();
	glm::vec2 markerPosition = _battleView->GetTerrainPosition3(screenMarkerPosition).xy();

	Unit* enemyUnit = FindEnemyUnit(touchPosition, markerPosition);
	glm::vec2 unitCenter = unit->state.center;

	bool isModifierMode = _tappedModiferArea || _modifierTouch != nullptr || _trackingTouch->GetCurrentButtons().right;
	_trackingMarker->SetRenderOrientation(isModifierMode);

	if (!isModifierMode)
	{
		if (enemyUnit && !_trackingMarker->GetMeleeTarget())
			SoundPlayer::singleton->Play(SoundBufferCommandMod);

		std::vector<glm::vec2>& path = _trackingMarker->_path;

		glm::vec2 currentDestination = path.size() != 0 ? *(path.end() - 1) : unit->state.center;

		bounds2f contentBounds = _battleView->GetContentBounds();
		glm::vec2 contentCenter = contentBounds.center();
		float contentRadius = contentBounds.width() / 2;

		glm::vec2 differenceToCenter = contentCenter - markerPosition;
		float distanceToCenter = glm::length(differenceToCenter);
		if (distanceToCenter > contentRadius)
		{
			markerPosition += differenceToCenter * (distanceToCenter - contentRadius) / distanceToCenter;
		}

		float waterEdgeFactor = -1;
		float delta = 2 / fmaxf(1, glm::length(currentDestination - markerPosition));
		for (float k = 0; k < 1; k += delta)
		{
			if (_battleView->GetBattleModel()->terrainSurface->IsImpassable(glm::mix(currentDestination, markerPosition, k)))
			{
				waterEdgeFactor = k;
				break;
			}
		}

		if (waterEdgeFactor >= 0)
		{
			glm::vec2 diff = markerPosition - currentDestination;
			markerPosition = currentDestination + diff * waterEdgeFactor;
			markerPosition -= glm::normalize(diff) * 10.0f;
		}


		_trackingMarker->SetMeleeTarget(enemyUnit);
		_trackingMarker->SetDestination(&markerPosition);

		if (enemyUnit != nullptr)
			MovementRules::UpdateMovementPath(_trackingMarker->_path, unitCenter, enemyUnit->state.center);
		else
			MovementRules::UpdateMovementPath(_trackingMarker->_path, unitCenter, markerPosition);

		if (enemyUnit != nullptr)
		{
			glm::vec2 destination = enemyUnit->state.center;
			glm::vec2 orientation = destination + glm::normalize(destination - unitCenter) * 18.0f;
			_trackingMarker->SetOrientation(&orientation);
		}
		else if (MovementRules::Length(_trackingMarker->_path) > KEEP_ORIENTATION_TRESHHOLD)
		{
			glm::vec2 dir = glm::normalize(markerPosition - unitCenter);
			if (path.size() >= 2)
				dir = glm::normalize(*(path.end() - 1) - *(path.end() - 2));
			glm::vec2 orientation = markerPosition + dir * 18.0f;
			_trackingMarker->SetOrientation(&orientation);
		}
		else
		{
			glm::vec2 orientation = markerPosition + 18.0f * vector2_from_angle(unit->state.direction);
			_trackingMarker->SetOrientation(&orientation);
		}
	}
	else
	{
		MovementRules::UpdateMovementPathStart(_trackingMarker->_path, unitCenter);

		bool holdFire = false;
		if (_trackingMarker->GetUnit()->state.unitMode == UnitModeStanding && _trackingMarker->GetUnit()->stats.maximumRange > 0)
		{
			bounds2f unitCurrentBounds = GetUnitCurrentBounds(_trackingMarker->GetUnit());
			holdFire = glm::distance(screenMarkerPosition, unitCurrentBounds.center()) <= unitCurrentBounds.x().radius();
		}

		if (holdFire)
		{
			_trackingMarker->GetUnit()->command.holdFire = true;
			_trackingMarker->SetMissileTarget(nullptr);
			_trackingMarker->SetOrientation(nullptr);
		}
		else
		{
			//if (!_tappedUnitCenter)
			//	enemyUnit = nullptr;
			if (!_allowTargetEnemyUnit)
				enemyUnit = nullptr;

			if (enemyUnit != nullptr && _trackingMarker->GetMissileTarget() == nullptr)
				SoundPlayer::singleton->Play(SoundBufferCommandMod);

			_trackingMarker->GetUnit()->command.holdFire = false;
			_trackingMarker->SetMissileTarget(enemyUnit);
			_trackingMarker->SetOrientation(&markerPosition);
		}
	}
}


Unit* BattleGesture::FindFriendlyUnit(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	if (disableUnitTracking)
		return nullptr;

	Unit* unitByPosition = FindFriendlyUnitByCurrentPosition(screenPosition, terrainPosition);
	Unit* unitByDestination = FindFriendlyUnitByFuturePosition(screenPosition, terrainPosition);
	Unit* unitByModifier = FindFriendlyUnitByModifierArea(screenPosition, terrainPosition);

	if (unitByPosition != nullptr && unitByDestination == nullptr)
	{
		return unitByPosition;
	}

	if (unitByPosition == nullptr && unitByDestination != nullptr)
	{
		return unitByDestination;
	}

	if (unitByPosition != nullptr && unitByDestination != nullptr)
	{
		float distanceToPosition = glm::length(unitByPosition->state.center - screenPosition);
		float distanceToDestination = glm::length(unitByDestination->command.GetDestination() - screenPosition);
		return distanceToPosition < distanceToDestination
				? unitByPosition
				: unitByDestination;
	}

	if (unitByModifier != nullptr)
		return unitByModifier;

	return nullptr;
}


Unit* BattleGesture::FindFriendlyUnitByCurrentPosition(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	Unit* result = nullptr;
	UnitCounter* unitMarker = _battleView->GetBattleModel()->GetNearestUnitCounter(terrainPosition, _battleView->_player);
	if (unitMarker != nullptr)
	{
		Unit* unit = unitMarker->_unit;
		if (!unit->state.IsRouting() && GetUnitCurrentBounds(unit).contains(screenPosition))
		{
			result = unit;
		}
	}
	return result;
}


Unit* BattleGesture::FindFriendlyUnitByFuturePosition(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	Unit* result = nullptr;
	UnitMovementMarker* movementMarker = _battleView->GetNearestMovementMarker(terrainPosition, _battleView->_player);
	if (movementMarker != nullptr)
	{
		Unit* unit = movementMarker->GetUnit();
		if (!unit->state.IsRouting() && GetUnitFutureBounds(unit).contains(screenPosition))
		{
			result = unit;
		}
	}
	return result;
}


Unit* BattleGesture::FindFriendlyUnitByModifierArea(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	Unit* result = nullptr;
	float distance = 10000;

	for (std::pair<int, Unit*> i : _battleView->GetBattleModel()->units)
	{
		Unit* unit = i.second;
		if (unit->player == _battleView->_player)
		{
			glm::vec2 center = !unit->command.path.empty() ? unit->command.path.back() : unit->state.center;
			float d = glm::distance(center, terrainPosition);
			if (d < distance && !unit->state.IsRouting() && GetUnitModifierBounds(unit).contains(screenPosition))
			{
				result = unit;
				distance = d;
			}
		}
	}

	return result;
}


Unit* BattleGesture::FindEnemyUnit(glm::vec2 touchPosition, glm::vec2 markerPosition)
{
	Player enemyPlayer = _trackingMarker->GetUnit()->player == Player1 ? Player2 : Player1;

	UnitCounter* enemyMarker = nullptr;

	glm::vec2 p = markerPosition;
	glm::vec2 d = (touchPosition - markerPosition) / 4.0f;
	for (int i = 0; i < 4; ++i)
	{
		UnitCounter* unitMarker = _battleView->GetBattleModel()->GetNearestUnitCounter(p, enemyPlayer);
		if (unitMarker && glm::length(unitMarker->_unit->state.center - p) <= SNAP_TO_UNIT_TRESHOLD)
		{
			enemyMarker = unitMarker;
			break;
		}
		p += d;
	}

	if (enemyMarker != nullptr)
		return enemyMarker->_unit;
	else
		return nullptr;
}


bounds2f BattleGesture::GetUnitCurrentBounds(Unit* unit)
{
	return _battleView->GetUnitCurrentIconViewportBounds(unit).grow(12);
}


bounds2f BattleGesture::GetUnitFutureBounds(Unit* unit)
{
	return _battleView->GetUnitFutureIconViewportBounds(unit).grow(12);
}


bounds2f BattleGesture::GetUnitModifierBounds(Unit* unit)
{
	switch (unit->state.unitMode)
	{
		case UnitModeStanding: return _battleView->GetUnitCurrentFacingMarkerBounds(unit).grow(12);
		case UnitModeMoving: return _battleView->GetUnitFutureFacingMarkerBounds(unit).grow(12);
		default: return bounds2f();
	}
}
