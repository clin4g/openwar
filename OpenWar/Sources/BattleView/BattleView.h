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

#include "BillboardRenderer.h"


class CasualtyMarker;
class MovementMarker;
class RangeMarker;
class ShootingCounter;
class TrackingMarker;
class UnitCounter;
class LineRenderer;
class ColorLineRenderer;
class ColorBillboardRenderer;

class BattleView : public TerrainView, public SimulationListener
{
	renderers* _renderers;
	BattleModel* _battleModel;

	BattleRendering* _battleRendering;

	glm::vec3 _lightNormal;

	BillboardTexture* _billboardTexture;
	BillboardModel* _billboardModel;
	BillboardRenderer* _billboardRenderer;


	CasualtyMarker* _casualtyMarker;
	std::vector<MovementMarker*> _movementMarkers;
	std::vector<TrackingMarker*> _trackingMarkers;

	LineRenderer* _lineRenderer;
	ColorLineRenderer* colorLineRenderer;
	ColorBillboardRenderer* _colorBillboardRenderer;

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

	MovementMarker* AddMovementMarker(Unit* unit);
	MovementMarker* GetMovementMarker(Unit* unit);
	MovementMarker* GetNearestMovementMarker(glm::vec2 position, Player player);

	TrackingMarker* AddTrackingMarker(Unit* unit);
	TrackingMarker* GetTrackingMarker(Unit* unit);
	void RemoveTrackingMarker(TrackingMarker* trackingMarker);

	void Initialize(bool editor = false);
	void InitializeTerrainShadow(BattleRendering* rendering);

	void InitializeTerrainTrees();
	void UpdateTerrainTrees(bounds2f bounds);

	void InitializeCameraPosition(const std::map<int, Unit*>& units);

	virtual void Render();
	virtual void Update(double secondsSinceLastUpdate);


	void RenderBackgroundLinen(BattleRendering* rendering);

	void RenderTerrainShadow(BattleRendering* rendering);
	void RenderTerrainGround(BattleRendering* rendering);

	void RenderRangeMarkers(BattleRendering* rendering);

	void RenderUnitMarkers(BattleRendering* rendering);
	void AppendUnitMarker(BattleRendering* rendering, UnitCounter* marker);

	void RenderUnitMissileTarget(BattleRendering* rendering, Unit* unit);

	void RenderTrackingMarkers(BattleRendering* rendering);
	void RenderTrackingMarker(BattleRendering* rendering, TrackingMarker* marker);
	void RenderTrackingShadow(BattleRendering* rendering, TrackingMarker* marker);
	void RenderTrackingPath(BattleRendering* rendering, TrackingMarker* marker);
	void RenderTrackingOrientation(BattleRendering* rendering, TrackingMarker* marker);

	void RenderMovementMarkers(BattleRendering* rendering);
	void RenderMovementMarker(BattleRendering* rendering, Unit* unit);
	void RenderMovementPath(BattleRendering* rendering, Unit* unit);



	static void TexRectN(vertexbuffer<texture_vertex>& shape, int size, int x, int y, int w, int h);
	static void TexRect256(vertexbuffer<texture_vertex>& shape, int x, int y, int w, int h);

	void TexRectN(vertexbuffer<texture_vertex3>& shape, int size, int x, int y, int w, int h);
	void TexRect256(vertexbuffer<texture_vertex3>& shape, int x, int y, int w, int h);

	void MissileLine(vertexbuffer<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, float scale);
	void MissileHead(vertexbuffer<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, float scale);
	void TexLine16(vertexbuffer<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, int t1, int t2, float scale);

	void _Path(vertexbuffer<texture_vertex3>& shape, int mode, float scale, const std::vector<glm::vec2>& path, float t0);
	void Path(vertexbuffer<texture_vertex3>& shape, int mode, glm::vec2 position, const std::vector<glm::vec2>& path, float t0);
};


#endif
