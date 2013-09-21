// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include "../../Library/Graphics/vertexbuffer.h"
#include "../../Library/Renderers/TextureBillboardRenderer.h"

#include "../BattleModel/BattleModel.h"
#include "../Simulator/BattleSimulator.h"
#include "../SmoothTerrain/SmoothTerrainSurfaceRenderer.h"
#include "../TerrainSurface/TiledTerrainSurfaceRenderer.h"
#include "../TerrainView/TerrainView.h"



class CasualtyMarker;
class ColorBillboardRenderer;
class GradientLineRenderer;
class GradientTriangleRenderer;
class GradientTriangleStripRenderer;
class UnitMovementMarker;
class PlainLineRenderer;
class PlainTriangleRenderer;
class RangeMarker;
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
	texture* _textureFacing;

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

	void Initialize();

	void InitializeTerrainTrees();
	void UpdateTerrainTrees(bounds2f bounds);

	void InitializeCameraPosition(const std::map<int, Unit*>& units);

	virtual void Render();
	virtual void Update(double secondsSinceLastUpdate);

	bounds2f GetBillboardBounds(glm::vec3 position, float height);

	bounds2f GetUnitCurrentIconViewportBounds(Unit* unit);
	bounds2f GetUnitFutureIconViewportBounds(Unit* unit);

	bounds2f GetUnitFacingMarkerBounds(glm::vec2 center, float direction);
	bounds2f GetUnitCurrentFacingMarkerBounds(Unit* unit);
	bounds2f GetUnitFutureFacingMarkerBounds(Unit* unit);

	bounds1f GetUnitIconSizeLimit() const;
};


#endif
