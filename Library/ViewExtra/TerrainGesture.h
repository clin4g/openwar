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
	bool _keyScrollLeft;
	bool _keyScrollRight;
	bool _keyScrollForward;
	bool _keyScrollBackward;
	bool _keyOrbitLeft;
	bool _keyOrbitRight;
	float _keyOrbitMomentum;
	glm::vec2 _keyScrollMomentum;

public:
	TerrainGesture(TerrainView* terrainView);
	virtual ~TerrainGesture();

	virtual void RenderHints();
	virtual void Update(double secondsSinceLastUpdate);

	virtual void KeyDown(Surface* surface, char key);
	virtual void KeyUp(Surface* surface, char key);

	virtual void ScrollWheel(Surface* surface, glm::vec2 position, glm::vec2 delta);
	virtual void Magnify(Surface* surface, glm::vec2 position, float magnification);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);

private:
	void UpdateMomentumOrbit(double secondsSinceLastUpdate);
	void UpdateMomentumScroll(double secondsSinceLastUpdate);
	void UpdateKeyScroll(double secondsSinceLastUpdate);
	void UpdateKeyOrbit(double secondsSinceLastUpdate);

	void MoveAndOrbit(Touch* touch);
	void ZoomAndOrbit(Touch* touch1, Touch* touch2);

	void ResetSamples(double timestamp);
	void UpdateSamples(double timestamp);

	glm::vec2 GetScrollVelocity() const;
	float GetOrbitVelocity() const;

	void AdjustToKeepInView(float adjustmentFactor, float secondsSinceLastUpdate);
};


#endif
