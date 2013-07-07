// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include "BattleModel.h"
#include "BattleSimulator.h"
#include "SmoothTerrainSurfaceRenderer.h"
#include "TiledTerrainSurfaceRenderer.h"
#include "TerrainView.h"
#include "vertexbuffer.h"

#include "TextureBillboardRenderer.h"


class CasualtyMarker;
class ColorBillboardRenderer;
class GradientLineRenderer;
class GradientTriangleRenderer;
class GradientTriangleStripRenderer;
class UnitMovementMarker;
class PlainLineRenderer;
class PlainTriangleRenderer;
class KillZoneMarker;
class ShootingCounter;
class TextureTriangleRenderer;
class UnitTrackingMarker;
class UnitCounter;


class BattleView : public TerrainView, public SimulationListener
{
	renderers* _renderers;
	BattleModel* _battleModel;

	glm::vec3 _lightNormal;

	BillboardTexture* _billboardTexture;
	BillboardModel* _billboardModel;
	TextureBillboardRenderer* _textureBillboardRenderer;
	TextureBillboardRenderer* _textureBillboardRenderer1;
	TextureBillboardRenderer* _textureBillboardRenderer2;

	CasualtyMarker* _casualtyMarker;
	std::vector<UnitMovementMarker*> _movementMarkers;
	std::vector<UnitTrackingMarker*> _trackingMarkers;

	PlainLineRenderer* _plainLineRenderer;
	PlainTriangleRenderer* _plainTriangleRenderer;
	GradientLineRenderer* _gradientLineRenderer;
	GradientTriangleRenderer* _gradientTriangleRenderer;
	GradientTriangleStripRenderer* _gradientTriangleStripRenderer;
	ColorBillboardRenderer* _colorBillboardRenderer;
	TextureTriangleRenderer* _textureTriangleRenderer;

	texture* _textureUnitMarkers;
	texture* _textureTouchMarker;

public:
	SmoothTerrainSurfaceRenderer* _terrainSurfaceRendererSmooth;
	TiledTerrainSurfaceRenderer* _terrainSurfaceRendererTiled;
	Player _player;

	BattleView(Surface* screen, BattleModel* battleModel, renderers* r);
	~BattleView();

	virtual BattleModel* GetBattleModel() const { return _battleModel; }
	virtual BattleView* GetBattleView() { return this; }
	virtual void OnShooting(const Shooting& shooting);
	virtual void OnCasualty(const Casualty& casualty);

	void AddCasualty(const Casualty& casualty);

	UnitMovementMarker* AddMovementMarker(Unit* unit);
	UnitMovementMarker* GetMovementMarker(Unit* unit);
	UnitMovementMarker* GetNearestMovementMarker(glm::vec2 position, Player player);

	UnitTrackingMarker* AddTrackingMarker(Unit* unit);
	UnitTrackingMarker* GetTrackingMarker(Unit* unit);
	void RemoveTrackingMarker(UnitTrackingMarker* trackingMarker);

	void Initialize(bool editor = false);

	void InitializeTerrainTrees();
	void UpdateTerrainTrees(bounds2f bounds);

	void InitializeCameraPosition(const std::map<int, Unit*>& units);

	virtual void Render();
	virtual void Update(double secondsSinceLastUpdate);

	bounds1f GetUnitIconSizeLimit() const;
};


#endif
