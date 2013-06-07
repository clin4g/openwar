/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include "BattleRendering.h"
#include "TerrainView.h"
#include "SimulationState.h"

#include "sprite.h"
#include "SmoothTerrainRendering.h"
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
	BattleModel* _boardModel;
	Player _bluePlayer;

	// static shapes
	shape<plain_vertex> _shape_water_inside;
	shape<plain_vertex> _shape_water_border;
	shape<plain_vertex> _shape_terrain_shadow;

	// dynamic shapes
	shape<BattleRendering::texture_billboard_vertex> _texture_billboards1;
	shape<BattleRendering::texture_billboard_vertex> _texture_billboards2;
	shape<BattleRendering::color_billboard_vertex> _color_billboards;

	std::vector<BattleRendering::texture_billboard_vertex> _static_billboards;
	std::vector<BattleRendering::texture_billboard_vertex> _dynamic_billboards;


	shape<plain_vertex3> _shape_fighter_weapons;
	shape<color_vertex3> _rangeMarker_shape;
	shape<texture_vertex3> _movementMarker_pathShape;

	shape<color_vertex3> _missileMarker_shape;

	shape<texture_vertex3> _trackingMarker_pathShape;
	shape<texture_vertex3> _trackingMarker_orientationShape;
	shape<texture_vertex3> _trackingMarker_missileHeadShape;
	shape<texture_vertex3> _unitMarker_targetLineShape;
	shape<texture_vertex3> _unitMarker_targetHeadShape;

	shape<texture_vertex> _trackingMarker_shadowShape;

	glm::vec3 _lightNormal;


public:
	SmoothTerrainRendering* _terrainRendering;

	BattleView(Surface* screen, BattleModel* boardModel, renderers* r, BattleRendering* battleRendering, SmoothTerrainRendering* terrainRendering, Player bluePlayer);
	~BattleView();

	BattleModel* GetBoardModel() const { return _boardModel; }

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
	void MakeRangeMarker(shape<color_vertex3>& shape, glm::vec2 position, float direction, float minimumRange, float maximumRange);

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

	static void TexRectN(shape<texture_vertex>& shape, int size, int x, int y, int w, int h);
	static void TexRect256(shape<texture_vertex>& shape, int x, int y, int w, int h);

	void TexRectN(shape<texture_vertex3>& shape, int size, int x, int y, int w, int h);
	void TexRect256(shape<texture_vertex3>& shape, int x, int y, int w, int h);

	void MissileLine(shape<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, float scale);
	void MissileHead(shape<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, float scale);
	void TexLine16(shape<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, int t1, int t2, float scale);

	void _Path(shape<texture_vertex3>& shape, int mode, float scale, const std::vector<glm::vec2>& path, float t0);
	void Path(shape<texture_vertex3>& shape, int mode, glm::vec2 position, const std::vector<glm::vec2>& path, float t0);
};


#endif
