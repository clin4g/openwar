// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include "BattleRendering.h"
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
class GradientTriangleStripRenderer;
class UnitMovementMarker;
class PlainLineRenderer;
class RangeMarker;
class ShootingCounter;
class TextureTriangleRenderer;
class UnitTrackingMarker;
class UnitCounter;


class BattleView : public TerrainView, public SimulationListener
{
	renderers* _renderers;
	BattleModel* _battleModel;

	BattleRendering* _battleRendering;

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
	GradientLineRenderer* _gradientLineRenderer;
	GradientTriangleStripRenderer* _gradientTriangleStripRenderer;
	ColorBillboardRenderer* _colorBillboardRenderer;
	TextureTriangleRenderer* _textureTriangleRenderer;

public:
	SmoothTerrainSurfaceRenderer* _terrainSurfaceRendererSmooth;
	TiledTerrainSurfaceRenderer* _terrainSurfaceRendererTiled;
	Player _player;

	BattleView(Surface* screen, BattleModel* battleModel, renderers* r, BattleRendering* battleRendering);
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

	void RenderUnitMissileTarget(BattleRendering* rendering, Unit* unit);

	void RenderTrackingShadow(BattleRendering* rendering, UnitTrackingMarker* marker);
	void RenderTrackingOrientation(BattleRendering* rendering, UnitTrackingMarker* marker);


	void TexRectN(vertexbuffer<texture_vertex>& shape, int size, int x, int y, int w, int h);

	void MissileLine(vertexbuffer<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, float scale);
	void MissileHead(vertexbuffer<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, float scale);
	void TexLine16(vertexbuffer<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, int t1, int t2, float scale);
};


#endif
