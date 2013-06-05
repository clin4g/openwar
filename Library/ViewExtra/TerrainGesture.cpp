/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "TerrainGesture.h"
#include "TerrainView.h"
#include "Touch.h"
#include "sprite.h"


TerrainGesture::TerrainGesture(TerrainView* terrainView) :
_terrainView(terrainView),
_previousCameraDirection(0),
_orbitAccumulator(0),
_orbitVelocity(0)
{
}


TerrainGesture::~TerrainGesture()
{

}


void TerrainGesture::RenderHints()
{
	if (this != nullptr)
		return;

	_terrainView->UseViewport();

	plain_shape shape;
	shape._mode = GL_LINES;

	plain_sprite sprite(renderers::singleton->_plain_renderer);
	sprite._shape = &shape;
	sprite._color = glm::vec4(0, 0, 0, 1);
	sprite._viewport = _terrainView->GetViewportBounds();

	glm::vec2 left = _terrainView->GetScreenLeft();
	glm::vec2 bottom = _terrainView->GetScreenBottom();
	glm::vec2 top = _terrainView->GetScreenTop();
	glm::vec2 right = _terrainView->GetScreenRight();

	bounds2f bounds(left.x, bottom.y, right.x, top.y);

	shape._vertices.push_back(plain_vertex(bounds.p12() + glm::vec2(0, 4)));
	shape._vertices.push_back(plain_vertex(bounds.p22() + glm::vec2(0, 4)));

	shape._vertices.push_back(plain_vertex(bounds.p11()));
	shape._vertices.push_back(plain_vertex(bounds.p12()));
	shape._vertices.push_back(plain_vertex(bounds.p12()));
	shape._vertices.push_back(plain_vertex(bounds.p22()));
	shape._vertices.push_back(plain_vertex(bounds.p22()));
	shape._vertices.push_back(plain_vertex(bounds.p21()));
	shape._vertices.push_back(plain_vertex(bounds.p21()));
	shape._vertices.push_back(plain_vertex(bounds.p11()));

	if (this != nullptr)
	{
		shape._vertices.push_back(plain_vertex(top + glm::vec2(0, 5)));
		shape._vertices.push_back(plain_vertex(top - glm::vec2(0, 5)));
		shape._vertices.push_back(plain_vertex(left + glm::vec2(5, 0)));
		shape._vertices.push_back(plain_vertex(left - glm::vec2(5, 0)));
		shape._vertices.push_back(plain_vertex(bottom + glm::vec2(0, 5)));
		shape._vertices.push_back(plain_vertex(bottom - glm::vec2(0, 5)));
		shape._vertices.push_back(plain_vertex(right + glm::vec2(5, 0)));
		shape._vertices.push_back(plain_vertex(right - glm::vec2(5, 0)));
	}

	glLineWidth(2);
	sprite.render();
	glLineWidth(1);
}


void TerrainGesture::Update(double secondsSinceLastUpdate)
{
	if (_touches.empty())
	{
		glm::vec2 screenPosition = _terrainView->ViewToScreen(glm::vec2(0, 0));
		glm::vec3 contentPosition = _terrainView->GetTerrainPosition2(screenPosition);

		_terrainView->Orbit(contentPosition.xy(), (float)secondsSinceLastUpdate * _orbitVelocity);

		contentPosition += (float)secondsSinceLastUpdate * glm::vec3(_scrollVelocity, 0);
		_terrainView->Move(contentPosition, screenPosition);

		_scrollVelocity = _scrollVelocity * exp2f(-4 * (float)secondsSinceLastUpdate);
		_orbitVelocity = _orbitVelocity * exp2f(-4 * (float)secondsSinceLastUpdate);

		AdjustToKeepInView(0.35f, (float)secondsSinceLastUpdate);
	}
}


void TerrainGesture::TouchBegan(Touch* touch)
{
	if (touch->GetSurface() != _terrainView->GetScreen())
		return;
	if (touch->HasGesture())
		return;
	if (!_terrainView->GetViewportBounds().contains(touch->GetPosition()))
		return;

	CaptureTouch(touch);

	_contentPosition1 = _terrainView->GetTerrainPosition3(_touches[0]->GetPosition());
	if (_touches.size() == 2)
		_contentPosition2 = _terrainView->GetTerrainPosition3(_touches[1]->GetPosition());

	_previousTouchPosition = touch->GetPosition();

	ResetSamples(touch->GetTimestamp());
}


static float adjust_toward_one(float value)
{
	float v2 = 1 - value;
	return 1 - v2 * v2;
}


static float GetOrbitFactor(Touch* touch, bounds2f bounds)
{
	glm::vec2 currentVelocity = touch->GetVelocity();
	float currentSpeed = glm::length(currentVelocity);

	float circularSteadiness = 0;
	if (currentSpeed > 0.01)
	{
		glm::vec2 centerDirection = glm::normalize(bounds.center() - touch->GetPosition());
		circularSteadiness = 1 - fabsf(glm::dot(currentVelocity, centerDirection)) / currentSpeed;
		circularSteadiness = adjust_toward_one(circularSteadiness);
	}

	float rectangularStediness = 0;
	glm::vec2 previousVelocity = touch->GetVelocity(touch->GetTimestamp() - 0.3);
	float previousSpeed = glm::length(previousVelocity);
	if (currentSpeed > 0.01 && previousSpeed > 0.01)
	{
		rectangularStediness = fabsf(glm::dot(currentVelocity, previousVelocity)) / currentSpeed / previousSpeed;
	}

	float positionFactor = glm::length((touch->GetPosition() - bounds.center()) / bounds.size());
	positionFactor = bounds1f(0, 1).clamp(14 * (positionFactor - 0.36f));

	//NSLog(@"circular: %f,  rectangular: %f,  position: %f", circularSteadiness, rectangularStediness, positionFactor);

	return circularSteadiness * (1 - rectangularStediness) * positionFactor;
}


void TerrainGesture::TouchMoved()
{
	if (_touches.size() == 1)
	{
		MoveAndOrbit(_touches[0]);
	}
	else if (_touches.size() == 2)
	{
		ZoomAndOrbit(_touches[0], _touches[1]);
	}

	UpdateSamples(_touches[0]->GetTimestamp());
}


void TerrainGesture::TouchEnded(Touch* touch)
{
	if (_touches.size() == 1)
	{
		_scrollVelocity = GetScrollVelocity();
		_orbitVelocity = GetOrbitVelocity();
	}
	else if (_touches.size() == 2)
	{
		Touch* other = touch == _touches[0] ? _touches[1] : _touches[0];
		_previousTouchPosition = other->GetPosition();
		_contentPosition1 = _terrainView->GetTerrainPosition3(_previousTouchPosition);
	}
}


void TerrainGesture::MoveAndOrbit(Touch* touch)
{
	bounds2f viewportBounds = _terrainView->GetViewportBounds();
	glm::vec2 touchPosition = touch->GetPosition();

	glm::vec2 centerScreen = _terrainView->ViewToScreen(glm::vec2(0, 0));
	glm::vec2 centerContent = _terrainView->GetTerrainPosition3(centerScreen).xy();
	glm::vec2 previousContent = _terrainView->GetTerrainPosition3(_previousTouchPosition).xy();
	glm::vec2 currentContent = _terrainView->GetTerrainPosition3(touchPosition).xy();

	glm::vec2 diff1 = currentContent - centerContent;
	glm::vec2 diff2 = previousContent - centerContent;

	float orbitFactor = GetOrbitFactor(touch, viewportBounds);
	float orbitAngle = orbitFactor * (angle(diff2) - angle(diff1));

	_terrainView->Orbit(centerContent, orbitAngle);
	_terrainView->Move(_contentPosition1, touchPosition);

	_contentPosition1 = _terrainView->GetTerrainPosition3(touchPosition);
	_previousTouchPosition = touch->GetPosition();

	AdjustToKeepInView(0.5, 0);
}


void TerrainGesture::ZoomAndOrbit(Touch* touch1, Touch* touch2)
{
	glm::vec2 v = glm::normalize(touch2->GetPosition() - touch1->GetPosition());
	glm::vec2 velocity1 = touch1->GetVelocity();
	glm::vec2 velocity2 = touch2->GetVelocity();
	float speed1 = glm::length(velocity1);
	float speed2 = glm::length(velocity2);
	float k1 = 1;
	float k2 = 1;
	if (speed1 > 0.01)
		k1 = glm::dot(v, velocity1) / speed1;
	if (speed2 > 0.01)
		k2 = glm::dot(v, velocity2) / speed2;
	float k = k1 * k2;
	float orbitFactor = 1 - k * k;

	_terrainView->Zoom(_contentPosition1, _contentPosition2, touch1->GetPosition(), touch2->GetPosition(), orbitFactor);

	AdjustToKeepInView(0.5, 0);
}


void TerrainGesture::ResetSamples(double timestamp)
{
	_previousCameraDirection = angle(_terrainView->GetCameraDirection().xy());
	_orbitAccumulator = 0;

	glm::vec2 screenPosition = _terrainView->ViewToScreen(glm::vec2(0, 0));
	glm::vec3 contentPosition = _terrainView->GetTerrainPosition2(screenPosition);

	_scrollSampler.clear();
	_scrollSampler.add(timestamp, contentPosition.xy());
	_orbitSampler.clear();
	_orbitSampler.add(timestamp, glm::vec2(_orbitAccumulator, 0));
}


void TerrainGesture::UpdateSamples(double timestamp)
{
	float currentCameraDirection = angle(_terrainView->GetCameraDirection().xy());
	float orbitDelta = angle_difference(currentCameraDirection, _previousCameraDirection);

	_previousCameraDirection = currentCameraDirection;

	_orbitAccumulator += orbitDelta;
	glm::vec2 screenPosition = _terrainView->ViewToScreen(glm::vec2(0, 0));
	glm::vec3 contentPosition = _terrainView->GetTerrainPosition2(screenPosition);

	_scrollSampler.add(timestamp, contentPosition.xy());
	_orbitSampler.add(timestamp, glm::vec2(_orbitAccumulator, 0));
}


glm::vec2 TerrainGesture::GetScrollVelocity() const
{
	double time = _scrollSampler.time();
	double dt = 0.1;
	glm::vec2 p2 = _scrollSampler.get(time);
	glm::vec2 p1 = _scrollSampler.get(time - dt);
	return (p2 - p1) / (float)dt;
}


float TerrainGesture::GetOrbitVelocity() const
{
	double time = _orbitSampler.time();
	double dt = 0.1;
	float v2 = _orbitSampler.get(time).x;
	float v1 = _orbitSampler.get(time - dt).x;

	return (float)((v2 - v1) / dt);
}


void TerrainGesture::AdjustToKeepInView(float adjustmentFactor, float secondsSinceLastUpdate)
{
	_terrainView->ClampCameraPosition();

	if (this != nullptr)
		return;

	bool is_scrolling = glm::length(_scrollVelocity) > 16;
	bool brake_scrolling = false;

	bounds2f viewportBounds = _terrainView->GetViewportBounds();
	glm::vec2 left = _terrainView->GetScreenLeft();
	glm::vec2 right = _terrainView->GetScreenRight();
	float dx1 = left.x - viewportBounds.min.x;
	float dx2 = viewportBounds.max.x - right.x;

	if (dx1 > 0 || dx2 > 0)
	{
		if (is_scrolling)
		{
			brake_scrolling = true;
		}
		else if (dx1 > 0 && dx2 > 0)
		{
			glm::vec3 contentPosition1 = _terrainView->GetTerrainPosition2(left);
			glm::vec3 contentPosition2 = _terrainView->GetTerrainPosition2(right);
			left.x -= adjustmentFactor * dx1;
			right.x += adjustmentFactor * dx2;
			_terrainView->Zoom(contentPosition1, contentPosition2, left, right, 0);
		}
		else if (dx1 > 0)
		{
			glm::vec3 contentPosition = _terrainView->GetTerrainPosition2(left);
			left.x -= adjustmentFactor * dx1;
			_terrainView->Move(contentPosition, left);
		}
		else if (dx2 > 0)
		{
			glm::vec3 contentPosition = _terrainView->GetTerrainPosition2(right);
			right.x += adjustmentFactor * dx2;
			_terrainView->Move(contentPosition, right);
		}
	}

	glm::vec2 bottom = _terrainView->GetScreenBottom();
	glm::vec2 top = _terrainView->GetScreenTop();
	float dy1 = bottom.y - viewportBounds.min.y;
	float dy2 = viewportBounds.min.y + 0.85f * viewportBounds.max.y - top.y;

	if (dy1 > 0 || dy2 > 0)
	{
		if (is_scrolling)
		{
			brake_scrolling = true;
		}
		else if (dy1 > 0 && dy2 > 0)
		{
			glm::vec3 contentPosition1 = _terrainView->GetTerrainPosition2(bottom);
			glm::vec3 contentPosition2 = _terrainView->GetTerrainPosition2(top);
			bottom.y -= adjustmentFactor * dy1;
			top.y += adjustmentFactor * dy2;
			_terrainView->Zoom(contentPosition1, contentPosition2, bottom, top, 0);
		}
		else if (dy1 > 0)
		{
			glm::vec3 contentPosition = _terrainView->GetTerrainPosition2(bottom);
			bottom.y -= adjustmentFactor * dy1;
			_terrainView->Move(contentPosition, bottom);
		}
		else if (dy2 > 0)
		{
			glm::vec3 contentPosition = _terrainView->GetTerrainPosition2(top);
			top.y += adjustmentFactor * dy2;
			_terrainView->Move(contentPosition, top);
		}
	}

	if (brake_scrolling)
	{
		_scrollVelocity = _scrollVelocity * exp2f(-16 * secondsSinceLastUpdate);
	}
}
