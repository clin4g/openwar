// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitTrackingMarker_H
#define UnitTrackingMarker_H

#include "UnitMarker.h"
#include "BattleModel.h"

class ColorBillboardRenderer;
class TextureBillboardRenderer;


class UnitTrackingMarker : public UnitMarker
{
	Unit* _meleeTarget;
	glm::vec2 _destination;
	bool _hasDestination;

	Unit* _missileTarget;
	glm::vec2 _orientation;
	bool _hasOrientation;
	bool _renderOrientation;

	bool _running;

public:
	std::vector<glm::vec2> _path;

public:
	UnitTrackingMarker(BattleModel* battleModel, Unit* unit);
	~UnitTrackingMarker();

	void SetRunning(bool value) { _running = value; }
	bool GetRunning() const { return _running; }

	void SetMeleeTarget(Unit* value) { _meleeTarget = value; }
	Unit* GetMeleeTarget() const { return _meleeTarget; }

	void SetMissileTarget(Unit* value) { _missileTarget = value; }
	Unit* GetMissileTarget() const { return _missileTarget; }

	/***/


	void SetDestination(glm::vec2* value)
	{
		if (value != nullptr) _destination = *value;
		_hasDestination = value !=  nullptr;
	}

	void SetOrientation(glm::vec2* value)
	{
		if (value != nullptr) _orientation = *value;
		_hasOrientation = value != nullptr;
	}

	void SetRenderOrientation(bool value) { _renderOrientation = value; }

	glm::vec2* GetOrientationX()
	{
		if (_missileTarget) return &_missileTarget->state.center;
		else if (_hasOrientation) return &_orientation;
		else return 0;
	}

	glm::vec2 DestinationXXX() const
	{
		return GetMeleeTarget() != nullptr ? GetMeleeTarget()->state.center
			: _path.size() != 0 ? *(_path.end() - 1)
			: _hasDestination ? _destination
			: GetUnit()->state.center;
	}


	void RenderTrackingFighters(ColorBillboardRenderer* renderer);
	void RenderTrackingMarker(TextureBillboardRenderer* renderer);
	void RenderTrackingShadow(TextureBillboardRenderer* renderer);
	void RenderTrackingPath(GradientTriangleRenderer* renderer);
	void RenderOrientation(GradientTriangleRenderer* renderer);
};


#endif
