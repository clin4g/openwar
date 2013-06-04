/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef TERRAINGESTURE_H
#define TERRAINGESTURE_H

#include "Gesture.h"
#include "sampler.h"

class TerrainView;


class TerrainGesture : public Gesture
{
	TerrainView* _terrainView;
	glm::vec3 _contentPosition1;
	glm::vec3 _contentPosition2;
	glm::vec2 _previousTouchPosition;

	sampler _scrollSampler;
	sampler _orbitSampler;
	glm::vec2 _scrollVelocity;
	float _previousCameraDirection;
	float _orbitAccumulator;
	float _orbitVelocity;

public:
	TerrainGesture(TerrainView* terrainView);
	virtual ~TerrainGesture();

	virtual void RenderHints();
	virtual void Update(double secondsSinceLastUpdate);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);

	void MoveAndOrbit(Touch* touch);
	void ZoomAndOrbit(Touch* touch1, Touch* touch2);

	void ResetSamples(double timestamp);
	void UpdateSamples(double timestamp);

	glm::vec2 GetScrollVelocity() const;
	float GetOrbitVelocity() const;

	void AdjustToKeepInView(float adjustmentFactor, float secondsSinceLastUpdate);
};


#endif
