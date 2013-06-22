// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleGesture.h"
#include "BattleModel.h"
#include "BattleView.h"
#include "SoundPlayer.h"
#include "UnitCounter.h"
#include "TrackingMarker.h"
#include "MovementMarker.h"

#include "sprite.h"


#define SNAP_TO_UNIT_TRESHOLD 22 // meters
#define KEEP_ORIENTATION_TRESHHOLD 40 // meters


bool BattleGesture::disableUnitTracking = false;


BattleGesture::BattleGesture(BattleView* battleView) :
_battleView(battleView),
_trackingMarker(0),
_trackingTouch(0),
_modifierTouch(0),
_tappedUnitCenter(false),
_tappedDestination(false),
_offsetToMarker(0),
_allowTargetEnemyUnit(false)
{
}


void BattleGesture::Update(double secondsSinceLastUpdate)
{

}


void BattleGesture::RenderHints()
{
	if (this != nullptr)
		return;

	_battleView->UseViewport();

	vertexbuffer<plain_vertex> shape;
	shape._mode = GL_LINES;

	plain_sprite sprite(renderers::singleton->_plain_renderer);
	sprite._shape = &shape;
	sprite._color = glm::vec4(0, 0, 0, 1);
	sprite._viewport = _battleView->GetViewportBounds();

	for (UnitCounter* unitMarker : _battleView->GetBattleModel()->_unitMarkers)
	{
		bounds2f bounds = GetUnitCurrentScreenBounds(unitMarker->_unit);

		shape._vertices.push_back(plain_vertex(bounds.p11()));
		shape._vertices.push_back(plain_vertex(bounds.p12()));
		shape._vertices.push_back(plain_vertex(bounds.p12()));
		shape._vertices.push_back(plain_vertex(bounds.p22()));
		shape._vertices.push_back(plain_vertex(bounds.p22()));
		shape._vertices.push_back(plain_vertex(bounds.p21()));
		shape._vertices.push_back(plain_vertex(bounds.p21()));
		shape._vertices.push_back(plain_vertex(bounds.p11()));

		bounds = GetUnitFutureScreenBounds(unitMarker->_unit);
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
}


void BattleGesture::TouchBegan(Touch* touch)
{
	if (touch->GetSurface() != _battleView->GetScreen())
		return;
	if (touch->GetGesture() != nullptr)
		return;
	if (!_battleView->GetViewportBounds().contains(touch->GetPosition()))
		return;

	glm::vec2 screenPosition = touch->GetPosition();
	glm::vec2 terrainPosition = _battleView->GetTerrainPosition3(screenPosition).xy();
	Unit* unit = FindNearestTouchUnit(screenPosition, terrainPosition);

	if (_trackingTouch == nullptr)
	{
		if (unit == nullptr)
			return;

		if (unit != nullptr && !_battleView->GetTrackingMarker(unit))
		{
			_allowTargetEnemyUnit = unit->stats.unitWeapon == UnitWeaponBow || unit->stats.unitWeapon == UnitWeaponArq;
			_trackingMarker = _battleView->AddTrackingMarker(unit);

			_tappedUnitCenter = GetUnitCurrentScreenBounds(unit).contains(screenPosition);
			_tappedDestination = GetUnitFutureScreenBounds(unit).contains(screenPosition);

			if (_tappedDestination)
			{
				_offsetToMarker = 0;//(_boardView->ContentToScreen(vector3(unit->movement.GetFinalDestination(), 0)).y - _boardView->ContentToScreen(vector3(terrainPosition, 0)).y) * GetFlipSign();
				if (_offsetToMarker < 0)
					_offsetToMarker = 0;

				std::vector<glm::vec2>& path = _trackingMarker->_path;
				path.clear();
				path.insert(path.begin(), unit->movement.path.begin(), unit->movement.path.end());
				_trackingMarker->SetDestination(&unit->movement.destination);

				glm::vec2 orientation = unit->movement.GetFinalDestination() + 18.0f * vector2_from_angle(unit->movement.direction);
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
				unit->movement.target = nullptr;
				unit->movement.path.clear();
				unit->movement.destination = unit->state.center;
				unit->missileTarget = nullptr;
				unit->missileTargetLocked = false;
			}

			_trackingMarker->_running = touch->GetTapCount() > 1 || (!_tappedUnitCenter && unit->movement.running);

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
		//static int* _icon_size = nullptr;
		//if (_icon_size == nullptr)
		//	_icon_size = new int([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone ? 57 : 72);

		//vector2 oldPosition = _trackingTouch->GetPrevious();//_boardView->GetTerrainPosition(_trackingTouch->_previous).xy();
		//vector2 newPosition = _trackingTouch->GetPosition();//_boardView->GetTerrainPosition(_trackingTouch->_position).xy();

		/*
		float diff = (newPosition.y - oldPosition.y) * GetFlipSign();
		if (diff < 0)
			_offsetToMarker -= diff / 2;
		else
			_offsetToMarker += diff;

		if (_offsetToMarker > *_icon_size / 2)
			_offsetToMarker = *_icon_size / 2;
		*/

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
			Unit* unit = _trackingMarker->_unit;

			Unit* destinationUnit = _trackingMarker->_destinationUnit;
			Unit* orientationUnit = _trackingMarker->_orientationUnit;
			glm::vec2* destination = _trackingMarker->GetDestinationX();
			glm::vec2* orientation = _trackingMarker->GetOrientationX();
			std::vector<glm::vec2>& path = _trackingMarker->_path;

			if (path.size() != 0)
			{
				unit->movement.path.clear();
				unit->movement.path.insert(unit->movement.path.begin(), path.begin(), path.end());
			}

			if (destinationUnit)
			{
				unit->movement.target = destinationUnit;
				unit->movement.destination = destinationUnit->state.center;
				unit->movement.running = false;
			}
			else if (destination)
			{
				unit->movement.target = nullptr;
				unit->movement.destination = *destination;
				unit->movement.running = _trackingMarker->_running;
			}

			if (orientationUnit)
			{
				unit->missileTarget = orientationUnit;
				unit->missileTargetLocked = true;
				unit->movement.direction = angle(orientationUnit->state.center - unit->movement.GetFinalDestination());
			}
			else if (orientation)
			{
				unit->movement.direction = angle(*orientation - unit->movement.GetFinalDestination());
			}

			if (!touch->HasMoved())
			{
				if (_tappedUnitCenter && touch->GetTapCount() > 1)
				{
					unit->movement.target = nullptr;
					unit->movement.path.clear();
					unit->movement.destination = unit->state.center;
					unit->missileTarget = nullptr;
					unit->missileTargetLocked = false;
				}
				else if (_tappedDestination && !_tappedUnitCenter)
				{
					if (!unit->movement.running)
					{
						unit->movement.running = true;
					}
				}
				else if (_tappedUnitCenter && !_tappedDestination)
				{
					if (unit->movement.running)
					{
						unit->movement.running = false;
					}
				}
			}

			unit->timeUntilSwapFighters = 0.2f;

			if (!_battleView->GetMovementMarker(unit))
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
		_trackingTouch = nullptr;
	else if (touch == _modifierTouch)
		_modifierTouch = nullptr;
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



Unit* BattleGesture::FindNearestTouchUnit(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	if (disableUnitTracking)
		return nullptr;

	Unit* unitByPosition = GetTouchedUnitMarker(screenPosition, terrainPosition);
	Unit* unitByDestination = GetTouchedMovementMarker(screenPosition, terrainPosition);

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
		float distanceToDestination = glm::length(unitByDestination->movement.GetFinalDestination() - screenPosition);
		return distanceToPosition < distanceToDestination
				? unitByPosition
				: unitByDestination;
	}

	return nullptr;
}


bounds2f BattleGesture::GetUnitCurrentScreenBounds(Unit* unit)
{
	glm::mat4x4 transform = _battleView->GetTransform();
	glm::vec4 position = transform * glm::vec4(_battleView->GetPosition(unit->state.center, 0), 1.0f);
	return bounds2_from_center(_battleView->ViewToScreen((glm::vec2)position.xy() / position.w), 32);
}


bounds2f BattleGesture::GetUnitFutureScreenBounds(Unit* unit)
{
	if (unit->movement.path.empty())
		return bounds2f();

	glm::mat4x4 transform = _battleView->GetTransform();
	glm::vec4 position = transform * glm::vec4(_battleView->GetPosition(unit->movement.path.back(), 0), 1.0f);
	return bounds2_from_center(_battleView->ViewToScreen((glm::vec2)position.xy() / position.w), 32);
}


Unit* BattleGesture::GetTouchedUnitMarker(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	Unit* result = nullptr;
	UnitCounter* unitMarker = _battleView->GetBattleModel()->GetNearestUnitMarker(terrainPosition, _battleView->_player);
	if (unitMarker != nullptr)
	{
		Unit* unit = unitMarker->_unit;
		if (!unit->state.IsRouting() && GetUnitCurrentScreenBounds(unit).contains(screenPosition))
		{
			result = unit;
		}
	}
	return result;
}


Unit* BattleGesture::GetTouchedMovementMarker(glm::vec2 screenPosition, glm::vec2 terrainPosition)
{
	Unit* result = nullptr;
	MovementMarker* movementMarker = _battleView->GetNearestMovementMarker(terrainPosition, _battleView->_player);
	if (movementMarker != nullptr)
	{
		Unit* unit = movementMarker->_unit;
		if (!unit->state.IsRouting() && GetUnitFutureScreenBounds(unit).contains(screenPosition))
		{
			result = unit;
		}
	}
	return result;
}


void BattleGesture::UpdateTrackingMarker()
{
	Unit* unit = _trackingMarker->_unit;

	glm::vec2 screenTouchPosition = _trackingTouch->GetPosition();
	glm::vec2 screenMarkerPosition = screenTouchPosition + glm::vec2(0, 1) * (_offsetToMarker * GetFlipSign());
	glm::vec2 touchPosition = _battleView->GetTerrainPosition3(screenTouchPosition).xy();
	glm::vec2 markerPosition = _battleView->GetTerrainPosition3(screenMarkerPosition).xy();

	Player enemyPlayer = _trackingMarker->_unit->player == Player1 ? Player2 : Player1;
	Unit* enemyUnit = FindUnit(touchPosition, markerPosition, enemyPlayer);

	glm::vec2 origin = unit->state.center;

	if (_modifierTouch == nullptr)
	{
		if (enemyUnit && !_trackingMarker->_destinationUnit)
			SoundPlayer::singleton->Play(SoundBufferCommandMod);

		std::vector<glm::vec2>& path = _trackingMarker->_path;

		glm::vec2 currentDestination = path.size() != 0 ? *(path.end() - 1) : unit->state.center;

		glm::vec2 differenceToCenter = glm::vec2(512, 512) - markerPosition;
		float distanceToCenter = glm::length(differenceToCenter);
		if (distanceToCenter > 512)
		{
			markerPosition += differenceToCenter * (distanceToCenter - 512) / distanceToCenter;
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


		_trackingMarker->_destinationUnit = enemyUnit;
		_trackingMarker->SetDestination(&markerPosition);


		float trackingVelocity = glm::length(_trackingTouch->GetVelocity() / 1024.0f);
		MovementRules::UpdateMovementPath(_trackingMarker->_path, origin, markerPosition, trackingVelocity);


		if (enemyUnit)
		{
			glm::vec2 destination = enemyUnit->state.center;
			glm::vec2 orientation = destination + glm::normalize(destination - origin) * 18.0f;
			_trackingMarker->SetOrientation(&orientation);
		}
		else if (glm::length(origin - markerPosition) > KEEP_ORIENTATION_TRESHHOLD)
		{
			glm::vec2 dir = glm::normalize(markerPosition - origin);
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
		if (!_tappedUnitCenter)
			enemyUnit = nullptr;
		if (!_allowTargetEnemyUnit)
			enemyUnit = nullptr;

		if (enemyUnit && !_trackingMarker->_orientationUnit)
			SoundPlayer::singleton->Play(SoundBufferCommandMod);

		_trackingMarker->_orientationUnit = enemyUnit;
		_trackingMarker->SetOrientation(&markerPosition);
	}
}


Unit* BattleGesture::FindUnit(glm::vec2 touchPosition, glm::vec2 markerPosition, Player player)
{
	UnitCounter* enemyMarker = nullptr;

	glm::vec2 p = markerPosition;
	glm::vec2 d = (touchPosition - markerPosition) / 4.0f;
	for (int i = 0; i < 4; ++i)
	{
		UnitCounter* unitMarker = _battleView->GetBattleModel()->GetNearestUnitMarker(p, player);
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
