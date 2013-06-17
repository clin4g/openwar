// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include "BattleRendering.h"
#include "TerrainView.h"
#include "SimulationState.h"

#include "sprite.h"
#include "SmoothTerrainRenderer.h"
#include "TiledTerrainRenderer.h"
#include "vertexbuffer.h"


class BattleModel;
class CasualtyMarker;
class MovementMarker;
class RangeMarker;
class ShootingMarker;
class TrackingMarker;
class UnitMarker;


class BattleView : public TerrainView
{
	renderers* _renderers;
	BattleRendering* _battleRendering;
	BattleModel* _battleModel;
	Player _bluePlayer;

	// static shapes
	vertexbuffer<plain_vertex> _shape_water_inside;
	vertexbuffer<plain_vertex> _shape_water_border;
	vertexbuffer<plain_vertex> _shape_terrain_shadow;

	// dynamic shapes
	vertexbuffer<BattleRendering::texture_billboard_vertex> _texture_billboards1;
	vertexbuffer<BattleRendering::texture_billboard_vertex> _texture_billboards2;
	vertexbuffer<BattleRendering::color_billboard_vertex> _color_billboards;

	std::vector<BattleRendering::texture_billboard_vertex> _static_billboards;
	std::vector<BattleRendering::texture_billboard_vertex> _dynamic_billboards;


	vertexbuffer<plain_vertex3> _shape_fighter_weapons;
	vertexbuffer<color_vertex3> _rangeMarker_shape;
	vertexbuffer<texture_vertex3> _movementMarker_pathShape;

	vertexbuffer<color_vertex3> _missileMarker_shape;

	vertexbuffer<texture_vertex3> _trackingMarker_pathShape;
	vertexbuffer<texture_vertex3> _trackingMarker_orientationShape;
	vertexbuffer<texture_vertex3> _trackingMarker_missileHeadShape;
	vertexbuffer<texture_vertex3> _unitMarker_targetLineShape;
	vertexbuffer<texture_vertex3> _unitMarker_targetHeadShape;

	vertexbuffer<texture_vertex> _trackingMarker_shadowShape;

	glm::vec3 _lightNormal;


public:
	SmoothTerrainRenderer* _smoothTerrainRendering;
	TiledTerrainRenderer* _tiledTerrainRenderer;

	BattleView(Surface* screen, BattleModel* boardModel, renderers* r, BattleRendering* battleRendering, Player bluePlayer);
	~BattleView();

	BattleModel* GetBattleModel() const { return _battleModel; }

	void Initialize(SimulationState* simulationState, bool editor = false);
	void InitializeTerrainShadow();

	void InitializeTerrainTrees();
	void UpdateTerrainTrees(bounds2f bounds);

	void InitializeTerrainWater(bool editor);

	void InitializeCameraPosition(const std::map<int, Unit*>& units);

	virtual void Render();
	virtual void Update(double secondsSinceLastUpdate);

	void RenderBackgroundLinen();
	void RenderBackgroundSky();

	void RenderTerrainShadow();
	void RenderTerrainGround();
	void RenderTerrainWater();

	void RenderFighterWeapons();
	void AppendFighterWeapons(Unit* unit);

	void AppendCasualtyBillboards();
	void AppendFighterBillboards();
	void AppendSmokeBillboards();
	void RenderTerrainBillboards();

	void RenderRangeMarkers();
	void MakeRangeMarker(vertexbuffer<color_vertex3>& shape, glm::vec2 position, float direction, float minimumRange, float maximumRange);

	void RenderUnitMarkers();
	void AppendUnitMarker(UnitMarker* marker);

	void RenderUnitMissileTarget(Unit* unit);

	void RenderTrackingMarkers();
	void RenderTrackingMarker(TrackingMarker* marker);
	void RenderTrackingShadow(TrackingMarker* marker);
	void RenderTrackingPath(TrackingMarker* marker);
	void RenderTrackingOrientation(TrackingMarker* marker);
	void RenderTrackingFighters(TrackingMarker* marker);

	void RenderMovementMarkers();
	void RenderMovementMarker(Unit* unit);
	void RenderMovementPath(Unit* unit);
	void RenderMovementFighters(Unit* unit);

	void RenderShootingMarkers();
	void AppendShootingMarker(ShootingMarker* marker);
	void AppendShootingMarkerArrow(glm::vec3 p1, glm::vec3 p2, float t);
	void AppendShootingMarkerBullet(glm::vec3 p1, glm::vec3 p2, float t);

	BattleRendering::texture_billboard_vertex MakeBillboardVertex(glm::vec2 position, float height, int i, int j, bool flipx = false, bool flipy = false);

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
