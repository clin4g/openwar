// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TERRAINVIEW_H
#define TERRAINVIEW_H

#include "../../Library/ViewCore/View.h"
#include "../../Library/Algebra/geometry.h"
#include "../TerrainModel/TerrainSurface.h"

class PlainLineRenderer;


class TerrainView : public View
{
	bounds2f _contentBounds;
	glm::vec3 _cameraPosition;
	float _cameraTilt;
	float _cameraFacing;
	bool _mouseHintVisible;
	glm::vec2 _mouseHintPosition;

protected:
	TerrainSurface* _terrainSurface;

public:
	TerrainView(Surface* screen, TerrainSurface* terrainSurfaceModel);
	virtual ~TerrainView();

	void ShowMouseHint(glm::vec2 position);
	void HideMouseHint();
	void RenderMouseHint(PlainLineRenderer* renderer);

	bounds2f GetContentBounds() const { return _contentBounds; }
	void SetContentBounds(bounds2f value);
	float GetContentRadius() const;

	glm::vec2 GetScreenTop() const;
	glm::vec2 GetScreenLeft() const;
	glm::vec2 GetScreenBottom() const;
	glm::vec2 GetScreenRight() const;

	glm::vec3 GetCameraPosition() const { return _cameraPosition; }

	glm::vec3 GetCameraDirection() const;
	glm::vec3 GetCameraUpVector() const;

	float GetCameraFacing() const { return _cameraFacing; }
	void SetCameraFacing(float value) { _cameraFacing = value; }

	glm::mat4x4 GetProjectionMatrix() const;
	glm::mat4x4 GetViewMatrix() const;
	glm::mat4x4 GetTransform() const { return GetProjectionMatrix() * GetViewMatrix(); }
	ray GetCameraRay(glm::vec2 screenPosition) const;
	glm::vec3 GetTerrainPosition2(glm::vec2 screenPosition) const;
	glm::vec3 GetTerrainPosition3(glm::vec2 screenPosition) const;

	void Move(glm::vec3 originalContentPosition, glm::vec2 currentScreenPosition);
	void Zoom(glm::vec3 originalContentPosition1, glm::vec3 originalContentPosition2, glm::vec2 currentScreenPosition1, glm::vec2 currentScreenPosition2, float orbitFactor);
	void Orbit(glm::vec2 originalContentPosition, float angle);

	void MoveCamera(glm::vec3 position);
	void ClampCameraPosition();

	glm::vec3 ScreenToContent(glm::vec2 value) const;
	glm::vec2 ContentToScreen(glm::vec3 value) const;

	glm::vec3 GetPosition(glm::vec2 p, float h = 1) { return _terrainSurface->GetPosition(p, h); };

};


#endif
