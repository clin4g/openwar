// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleView.h"
#include "UnitCounter.h"
#include "CasualtyMarker.h"
#include "SmokeCounter.h"
#include "RangeMarker.h"
#include "TrackingMarker.h"
#include "MovementMarker.h"
#include "ShootingCounter.h"
#include "GradientRenderer.h"
#include "ColorBillboardRenderer.h"
#include "SmoothTerrainWater.h"
#include "SmoothTerrainSky.h"
#include "sprite.h"
#import "PlainRenderer.h"


static affine2 billboard_texcoords(int x, int y, bool flip)
{
	float x0 = 0.125f * x;
	float x1 = x0 + 0.125f;
	float y0 = 0.125f * y;
	float y1 = y0 + 0.125f;
	return flip ? affine2(glm::vec2(x1, y0), glm::vec2(x0, y1)) : affine2(glm::vec2(x0, y0), glm::vec2(x1, y1));
}



BattleView::BattleView(Surface* screen, BattleModel* battleModel, renderers* r, BattleRendering* battleRendering) : TerrainView(screen, battleModel->terrainSurface),
_renderers(r),
_battleRendering(battleRendering),
_battleModel(battleModel),
_terrainSurfaceRendererSmooth(nullptr),
_terrainSurfaceRendererTiled(nullptr),
_billboardTexture(nullptr),
_billboardModel(nullptr),
_textureBillboardRenderer(nullptr),
_textureBillboardRenderer1(nullptr),
_textureBillboardRenderer2(nullptr),
_casualtyMarker(0),
_movementMarkers(),
_trackingMarkers(),
_player(PlayerNone),
_plainLineRenderer(nullptr),
_gradientLineRenderer(nullptr),
_gradientTriangleStripRenderer(nullptr),
_colorBillboardRenderer(nullptr)
{
	SetContentBounds(bounds2f(0, 0, 1024, 1024));

	_billboardTexture = new BillboardTexture();
	_billboardTexture->AddSheet(image(@"Billboards.png"));

	_billboardModel = new BillboardModel();
	_billboardModel->texture = _billboardTexture;


	for (int i = 0; i < 8; ++i)
	{
		_billboardModel->_billboardTreeShapes[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardTreeShapes[i], 0, billboard_texcoords(i, 0, false));

		_billboardModel->_billboardTreeShapes[i + 8] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardTreeShapes[i + 8], 0, billboard_texcoords(i, 0, true));

		_billboardModel->_billboardShapeCasualtyCav[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyCav[i], 0, billboard_texcoords(i, 4, false));

		_billboardModel->_billboardShapeCasualtyCav[i + 8] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyCav[i + 8], 0, billboard_texcoords(i, 4, true));
	}

	for (int i = 0; i < 4; ++i)
	{
		_billboardModel->_billboardShapeCasualtyAsh[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyAsh[i], 0, billboard_texcoords(i, 3, false));

		_billboardModel->_billboardShapeCasualtyAsh[i + 4] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyAsh[i + 4], 0, billboard_texcoords(i, 3, false));

		_billboardModel->_billboardShapeCasualtySam[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtySam[i], 0, billboard_texcoords(i + 4, 3, false));

		_billboardModel->_billboardShapeCasualtySam[i + 4] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtySam[i + 4], 0, billboard_texcoords(i + 4, 3, false));
	}

	_billboardModel->_billboardShapeFighterSamBlue = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamBlue, 0, billboard_texcoords(1, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamBlue, 180, billboard_texcoords(2, 1, false));

	_billboardModel->_billboardShapeFighterSamRed = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamRed, 0, billboard_texcoords(1, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamRed, 180, billboard_texcoords(2, 2, false));

	_billboardModel->_billboardShapeFighterAshBlue = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshBlue, 0, billboard_texcoords(3, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshBlue, 180, billboard_texcoords(4, 1, false));

	_billboardModel->_billboardShapeFighterAshRed = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshRed, 0, billboard_texcoords(3, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshRed, 180, billboard_texcoords(4, 2, false));

	_billboardModel->_billboardShapeFighterCavBlue = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue,  45, billboard_texcoords(7, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue,  90, billboard_texcoords(6, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 135, billboard_texcoords(5, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 225, billboard_texcoords(5, 1, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 270, billboard_texcoords(6, 1, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 315, billboard_texcoords(7, 1, true));

	_billboardModel->_billboardShapeFighterCavRed = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed,  45, billboard_texcoords(7, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed,  90, billboard_texcoords(6, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 135, billboard_texcoords(5, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 225, billboard_texcoords(5, 2, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 270, billboard_texcoords(6, 2, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 315, billboard_texcoords(7, 2, true));


	for (int i = 0; i < 8; ++i)
	{
		_billboardModel->_billboardShapeSmoke[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeSmoke[i], 0, billboard_texcoords(i, 7, false));
	}

	_textureBillboardRenderer = new TextureBillboardRenderer();
	_textureBillboardRenderer1 = new TextureBillboardRenderer();
	_textureBillboardRenderer2 = new TextureBillboardRenderer();

	_casualtyMarker = new CasualtyMarker(_battleModel);

	_plainLineRenderer = new PlainLineRenderer();
	_gradientLineRenderer = new GradientLineRenderer();
	_gradientTriangleStripRenderer = new GradientTriangleStripRenderer();
	_colorBillboardRenderer = new ColorBillboardRenderer();
}


BattleView::~BattleView()
{
	delete _casualtyMarker;

	for (MovementMarker* marker : _movementMarkers)
		delete marker;

	for (TrackingMarker* marker : _trackingMarkers)
		delete marker;
}


void BattleView::OnShooting(Shooting const & shooting)
{
	_battleModel->AddShootingAndSmokeCounters(shooting);
}


void BattleView::OnCasualty(Casualty const & casualty)
{
	AddCasualty(casualty);
}


void BattleView::AddCasualty(const Casualty& casualty)
{
	glm::vec3 position = glm::vec3(casualty.position, _battleModel->terrainSurface->GetHeight(casualty.position));
	_casualtyMarker->AddCasualty(position, casualty.player, casualty.platform);
}


static float random_float()
{
	return (float)(rand() & 0x7fff) / 0x7fff;
}


void BattleView::Initialize(bool editor)
{
	InitializeTerrainShadow(_battleRendering);
	InitializeTerrainTrees();

	InitializeCameraPosition(_battleModel->units);
}


void BattleView::InitializeTerrainShadow(BattleRendering* rendering)
{
	bounds2f bounds = GetContentBounds();
	glm::vec2 center = bounds.center();
	float radius1 = 512;
	float radius2 = 550;

	rendering->_vboTerrainShadow._mode = GL_TRIANGLES;
	rendering->_vboTerrainShadow._vertices.clear();

	int n = 16;
	for (int i = 0; i < n; ++i)
	{
		float angle1 = i * 2 * (float)M_PI / n;
		float angle2 = (i + 1) * 2 * (float)M_PI / n;

		glm::vec2 p1 = center + radius1 * vector2_from_angle(angle1);
		glm::vec2 p2 = center + radius2 * vector2_from_angle(angle1);
		glm::vec2 p3 = center + radius2 * vector2_from_angle(angle2);
		glm::vec2 p4 = center + radius1 * vector2_from_angle(angle2);

		rendering->_vboTerrainShadow._vertices.push_back(plain_vertex(p1));
		rendering->_vboTerrainShadow._vertices.push_back(plain_vertex(p2));
		rendering->_vboTerrainShadow._vertices.push_back(plain_vertex(p3));
		rendering->_vboTerrainShadow._vertices.push_back(plain_vertex(p3));
		rendering->_vboTerrainShadow._vertices.push_back(plain_vertex(p4));
		rendering->_vboTerrainShadow._vertices.push_back(plain_vertex(p1));
	}

	rendering->_vboTerrainShadow.update(GL_STATIC_DRAW);
}


void BattleView::InitializeTerrainTrees()
{
	UpdateTerrainTrees(bounds2f(0, 0, 1024, 1024));
}


struct random_generator
{
	int _count;
	float* _values;

	random_generator(int count) : _count(count), _values(new float [count])
	{
		for (int i = 0; i < count; ++i)
			_values[i] = random_float();
	}
};

struct random_iterator
{
	const random_generator& _gen;
	int _index;
	random_iterator(const random_generator& gen) : _gen(gen), _index(0) { }
	float next()
	{
		float result = _gen._values[_index++];
		if (_index == _gen._count)
			_index = 0;
		return result;
	}
};


void BattleView::UpdateTerrainTrees(bounds2f bounds)
{
	if (_terrainSurfaceRendererSmooth != nullptr)
	{
		image* map = _terrainSurfaceRendererSmooth->GetTerrainSurfaceModel()->GetMap();


		auto pos2 = std::remove_if(_billboardModel->staticBillboards.begin(), _billboardModel->staticBillboards.end(), [bounds](const Billboard& billboard) {
			return bounds.contains(billboard.position.xy());
		});
		_billboardModel->staticBillboards.erase(pos2, _billboardModel->staticBillboards.end());


		static random_generator* _randoms = nullptr;
		if (_randoms == nullptr)
			_randoms = new random_generator(997);

		int treeType = 0;
		random_iterator random(*_randoms);

		float d = 5;
		for (float x = 0; x < 1024; x += d)
			for (float y = 0; y < 1024; y += d)
			{
				float dx = d * (random.next() - 0.5f);
				float dy = d * (random.next() - 0.5f);
				int shape = (int)(15 * random.next()) & 15;

				glm::vec2 position = glm::vec2(x + dx, y + dy);
				if (bounds.contains(position) && glm::length(position - 512.0f) < 512.0f)
				{
					float z = _terrainSurface->GetHeight(position);
					if (z > 0
							&& map->get_pixel((int)(position.x / 2), (int)(position.y / 2)).g > 0.5
							&& _terrainSurface->GetNormal(position).z >= 0.84)
					{
						const float adjust = 0.5 - 2.0 / 64.0; // place texture 2 texels below ground
						_billboardModel->staticBillboards.push_back(Billboard(GetPosition(position, adjust * 5), 0, 5, _billboardModel->_billboardTreeShapes[shape]));
					}
				}

				++treeType;
			}
	}
}



void BattleView::InitializeCameraPosition(const std::map<int, Unit*>& units)
{
	glm::vec2 friendlyCenter;
	glm::vec2 enemyCenter;
	int friendlyCount = 0;
	int enemyCount = 0;

	for (std::pair<int, Unit*> item : units)
	{
		Unit* unit = item.second;
		if (!unit->state.IsRouting())
		{
			if (unit->player == _player)
			{
				friendlyCenter += unit->state.center;
				++friendlyCount;
			}
			else
			{
				enemyCenter += unit->state.center;
				++enemyCount;
			}
		}
	}

	if (friendlyCount != 0 && enemyCount != 0)
	{
		friendlyCenter /= friendlyCount;
		enemyCenter /= enemyCount;

		glm::vec2 friendlyScreen = ViewToScreen(glm::vec2(0, GetFlip() ? 0.15 : -0.15));
		glm::vec2 enemyScreen = ViewToScreen(glm::vec2(0, GetFlip() ? -0.65 : 0.65));

		Zoom(glm::vec3(friendlyCenter, 0), glm::vec3(enemyCenter, 0), friendlyScreen, enemyScreen, 0);

		ClampCameraPosition();
	}
}


void BattleView::Render()
{
	UseViewport();

	glm::vec2 facing = vector2_from_angle(GetCameraFacing() - 2.5f * (float)M_PI_4);
	_lightNormal = glm::normalize(glm::vec3(facing, -1));

	_billboardModel->dynamicBillboards.clear();

	glDisable(GL_DEPTH_TEST);


	if (_battleModel->terrainSky != nullptr)
	{
		_battleModel->terrainSky->RenderBackgroundLinen(_renderers, GetViewportBounds(), GetFlip());
		_battleModel->terrainSky->Render(_renderers, GetCameraDirection().z, GetFlip());
	}

	RenderTerrainShadow(_battleRendering);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	RenderTerrainGround(_battleRendering);
	glDisable(GL_CULL_FACE);

	if (_battleModel->terrainWater != nullptr)
		_battleModel->terrainWater->Render(GetTransform());

	glDepthMask(false);

	_plainLineRenderer->Reset();
	for (UnitCounter* marker : _battleModel->_unitMarkers)
		marker->AppendFighterWeapons(_plainLineRenderer);
	_plainLineRenderer->Draw(GetTransform(), glm::vec4(0.4, 0.4, 0.4, 0.6));


	_colorBillboardRenderer->Reset();
	_casualtyMarker->RenderCasualtyColorBillboards(_colorBillboardRenderer);
	_colorBillboardRenderer->Draw(GetTransform(), GetCameraUpVector(), GetViewportBounds().height());


	_casualtyMarker->AppendCasualtyBillboards(_billboardModel);
	for (UnitCounter* marker : _battleModel->_unitMarkers)
		marker->AppendFighterBillboards(_billboardModel);
	for (SmokeCounter* marker : _battleModel->_smokeMarkers)
		marker->AppendSmokeBillboards(_billboardModel);
	_textureBillboardRenderer->Render(_billboardModel, GetTransform(), GetCameraUpVector(), glm::degrees(GetCameraFacing()));


	RenderRangeMarkers(_battleRendering);


	for (UnitCounter* marker : _battleModel->_unitMarkers)
	{
		Unit* unit = marker->_unit;
		RenderUnitMissileTarget(_battleRendering, unit);
	}

	_textureBillboardRenderer1->Reset();
	_textureBillboardRenderer2->Reset();

	for (UnitCounter* marker : _battleModel->_unitMarkers)
		marker->AppendUnitMarker(_textureBillboardRenderer1, _textureBillboardRenderer2, GetFlip());

	bounds1f sizeLimit = GetUnitIconSizeLimit();

	glDisable(GL_DEPTH_TEST);
	_textureBillboardRenderer1->Draw(_battleRendering->_textureUnitMarkers, GetTransform(), GetCameraUpVector(), glm::degrees(GetCameraFacing()), sizeLimit);
	_textureBillboardRenderer2->Draw(_battleRendering->_textureUnitMarkers, GetTransform(), GetCameraUpVector(), glm::degrees(GetCameraFacing()), sizeLimit);
	glEnable(GL_DEPTH_TEST);


	RenderTrackingMarkers(_battleRendering);

	_colorBillboardRenderer->Reset();
	for (TrackingMarker* marker : _trackingMarkers)
		marker->RenderTrackingFighters(_colorBillboardRenderer);
	_colorBillboardRenderer->Draw(GetTransform(), GetCameraUpVector(), GetViewportBounds().height());

	RenderMovementMarkers(_battleRendering);

	_colorBillboardRenderer->Reset();
	for (MovementMarker* marker : _movementMarkers)
		marker->RenderMovementFighters(_colorBillboardRenderer);
	_colorBillboardRenderer->Draw(GetTransform(), GetCameraUpVector(), GetViewportBounds().height());


	_gradientLineRenderer->Reset();
	for (ShootingCounter* shootingCounter : _battleModel->_shootingCounters)
		shootingCounter->Render(_gradientLineRenderer);
	_gradientLineRenderer->Draw(GetTransform());


	glDepthMask(true);

	glDisable(GL_DEPTH_TEST);
}


template <class T> void AnimateMarkers(std::vector<T*>& markers, float seconds)
{
	size_t index = 0;
	while (index < markers.size())
	{
		T* marker = markers[index];
		if (marker->Animate(seconds))
		{
			++index;
		}
		else
		{
			markers.erase(markers.begin() + index);
			delete marker;
		}
	}
}


void BattleView::Update(double secondsSinceLastUpdate)
{
	_casualtyMarker->Animate((float)secondsSinceLastUpdate);

	::AnimateMarkers(_movementMarkers, (float)secondsSinceLastUpdate);
}


MovementMarker* BattleView::AddMovementMarker(Unit* unit)
{
	MovementMarker* marker = new MovementMarker(_battleModel, unit);
	_movementMarkers.push_back(marker);
	return marker;
}


MovementMarker* BattleView::GetMovementMarker(Unit* unit)
{
	for (MovementMarker* marker : _movementMarkers)
		if (marker->_unit == unit)
			return marker;

	return 0;
}


MovementMarker* BattleView::GetNearestMovementMarker(glm::vec2 position, Player player)
{
	MovementMarker* result = 0;
	float nearest = INFINITY;

	for (MovementMarker* marker : _movementMarkers)
	{
		Unit* unit = marker->_unit;
		if (player != PlayerNone && unit->player != player)
			continue;

		glm::vec2 p = unit->movement.GetFinalDestination();
		float dx = p.x - position.x;
		float dy = p.y - position.y;
		float d = dx * dx + dy * dy;
		if (d < nearest)
		{
			result = marker;
			nearest = d;
		}
	}

	return result;
}


TrackingMarker* BattleView::AddTrackingMarker(Unit* unit)
{
	TrackingMarker* trackingMarker = new TrackingMarker(_battleModel, unit);
	_trackingMarkers.push_back(trackingMarker);
	return trackingMarker;
}


TrackingMarker* BattleView::GetTrackingMarker(Unit* unit)
{
	for (TrackingMarker* marker : _trackingMarkers)
		if (marker->_unit == unit)
			return marker;

	return 0;
}


void BattleView::RemoveTrackingMarker(TrackingMarker* trackingMarker)
{
	auto i = std::find(_trackingMarkers.begin(), _trackingMarkers.end(), trackingMarker);
	if (i != _trackingMarkers.end())
	{
		_trackingMarkers.erase(i);
		delete trackingMarker;
	}
}



void BattleView::RenderTerrainShadow(BattleRendering* rendering)
{
	plain_uniforms uniforms;
	uniforms._transform = GetTransform();

	rendering->_ground_shadow_renderer->render(rendering->_vboTerrainShadow, uniforms);
}


void BattleView::RenderTerrainGround(BattleRendering* rendering)
{
	if (_terrainSurfaceRendererSmooth != nullptr)
		_terrainSurfaceRendererSmooth->Render(GetTransform(), _lightNormal);

	if (_terrainSurfaceRendererTiled != nullptr)
		_terrainSurfaceRendererTiled->Render(GetTransform(), _lightNormal);
}



void BattleView::RenderRangeMarkers(BattleRendering* rendering)
{
	for (std::pair<int, Unit*> item : _battleModel->units)
	{
		RangeMarker marker(_battleModel, item.second);

		_gradientTriangleStripRenderer->Reset();
		marker.Render(_gradientTriangleStripRenderer);
		_gradientTriangleStripRenderer->Draw(GetTransform());
	}
}


static bool is_iphone()
{
	static bool* _is_iphone = nullptr;
	if (_is_iphone == nullptr)
	{
#if TARGET_OS_IPHONE
		_is_iphone = new bool([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone);
#else
		_is_iphone = new bool(false);
#endif
	}
	return *_is_iphone;
}



bounds1f BattleView::GetUnitIconSizeLimit() const
{
	float y = GetCameraDirection().z;
	float x = sqrtf(1 - y * y);
	float a = 1 - fabsf(atan2f(y, x) / (float)M_PI_2);

	bounds1f result(0, 0);
	result.min = (32 - 8 * a) * renderer_base::pixels_per_point();
	result.max = (32 + 16 * a) * renderer_base::pixels_per_point();
	if (is_iphone())
	{
		result.min *= 57.0f / 72.0f;
		result.max *= 57.0f / 72.0f;
	}
	return result;
}



void BattleView::RenderUnitMissileTarget(BattleRendering* rendering, Unit* unit)
{
	float scale = 0.5;

	if (unit->missileTarget != nullptr && unit->missileTargetLocked)
	{
		BattleView::MissileLine(rendering->_vboUnitMarkerTargetLine, unit->state.center, unit->missileTarget->state.center, scale);
		BattleView::MissileHead(rendering->_vboUnitMarkerTargetHead, unit->state.center, unit->missileTarget->state.center, scale);

		BattleRendering::ground_texture_uniforms uniforms;
		uniforms._transform = GetTransform();
		uniforms._texture = unit->player == _battleModel->bluePlayer ? rendering->_textureMissileBlue : rendering->_textureMissileRed;;

		rendering->_ground_texture_renderer->render(rendering->_vboUnitMarkerTargetLine, uniforms);
		rendering->_ground_texture_renderer->render(rendering->_vboUnitMarkerTargetHead, uniforms);
	}
}

static glm::vec2 DestinationXXX(TrackingMarker* marker)
{
	return marker->_destinationUnit ? marker->_destinationUnit->state.center
			: marker->_path.size() != 0 ? *(marker->_path.end() - 1)
					: marker->_hasDestination ? marker->_destination
							: marker->_unit->state.center;
}



void BattleView::RenderTrackingMarkers(BattleRendering* rendering)
{
	for (TrackingMarker* marker : _trackingMarkers)
	{
		glDisable(GL_DEPTH_TEST);
		RenderTrackingMarker(rendering, marker);
		RenderTrackingShadow(rendering, marker);
		glEnable(GL_DEPTH_TEST);

		RenderTrackingPath(rendering, marker);
		RenderTrackingOrientation(rendering, marker);
	}
}




void BattleView::RenderTrackingMarker(BattleRendering* rendering, TrackingMarker* marker)
{
	_textureBillboardRenderer1->Reset();

	if ((marker->_destinationUnit || marker->_hasDestination) && marker->_destinationUnit == nullptr)
	{

		glm::vec2 destination = DestinationXXX(marker);
		glm::vec3 position = GetPosition(destination, 0);
		glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
		glm::vec2 texcoord = texsize * glm::vec2(marker->_unit->player != _battleModel->bluePlayer ? 4 : 3, 0);

		_textureBillboardRenderer1->AddBillboard(position, 32, affine2(texcoord, texcoord + texsize));

	}

	bounds1f sizeLimit = bounds1f(24 * renderer_base::pixels_per_point(), 48 * renderer_base::pixels_per_point());
	_textureBillboardRenderer1->Draw(rendering->_textureUnitMarkers, GetTransform(), GetCameraUpVector(), glm::degrees(GetCameraFacing()), sizeLimit);
}


void BattleView::RenderTrackingShadow(BattleRendering* rendering, TrackingMarker* marker)
{
	glm::vec2 p = DestinationXXX(marker);
	glm::vec2 offset = ContentToScreen(GetPosition(p, 0));
	float scale = 5;

	TexRectN(rendering->_vboTrackingMarkerShadow, 32, 0, 0, 32, 32);

	for (texture_vertex& vertex : rendering->_vboTrackingMarkerShadow._vertices)
	{
		vertex._position *= scale;
		vertex._position += offset;
	}


	texture_uniforms uniforms;
	uniforms._transform = sprite_transform(GetViewportBounds()).transform();
	uniforms._texture = rendering->_textureTouchMarker;
	_renderers->_texture_renderer->render(rendering->_vboTrackingMarkerShadow, uniforms);
}


void BattleView::RenderTrackingPath(BattleRendering* rendering, TrackingMarker* marker)
{
	if (marker->_path.size() != 0)
	{
		glm::vec2 position = marker->_unit->state.center;

		int mode = 0;
		if (marker->_destinationUnit)
			mode = 2;
		else if (marker->_running)
			mode = 1;

		std::vector<glm::vec2> path(marker->_path);
		if (marker->_destinationUnit != 0)
			path.insert(path.end(), marker->_destinationUnit->state.center);

		BattleView::Path(rendering->_vboTrackingMarkerPath, mode, position, path, 0);

		BattleRendering::ground_texture_uniforms uniforms;
		uniforms._transform = GetTransform();
		uniforms._texture = rendering->_textureMovementGray;
		rendering->_ground_texture_renderer->render(rendering->_vboTrackingMarkerPath, uniforms);
	}
}


void BattleView::RenderTrackingOrientation(BattleRendering* rendering, TrackingMarker* marker)
{
	if (marker->_orientationUnit != nullptr || marker->_hasOrientation)
	{
		glm::vec2 destination = DestinationXXX(marker);
		glm::vec2 orientation = marker->_orientationUnit ? marker->_orientationUnit->state.center : marker->_orientation;

		BattleView::MissileLine(rendering->_vboTrackingMarkerOrientation, destination, orientation, 0.5);
		BattleView::MissileHead(rendering->_vboTrackingMarkerMissileHead, destination, orientation, 0.5);

		BattleRendering::ground_texture_uniforms uniforms;
		uniforms._transform = GetTransform();
		uniforms._texture = rendering->_textureMissileGray;

		//_ground_texture_renderer3->render(_trackingMarker_orientationShape, uniforms);

		if (marker->_orientationUnit != nullptr)
			rendering->_ground_texture_renderer->render(rendering->_vboTrackingMarkerMissileHead, uniforms);
	}
}


void BattleView::RenderMovementMarkers(BattleRendering* rendering)
{
	for (MovementMarker* marker : _movementMarkers)
	{
		glDisable(GL_DEPTH_TEST);
		RenderMovementMarker(rendering, marker->_unit);
		glEnable(GL_DEPTH_TEST);
		RenderMovementPath(rendering, marker->_unit);
	}
}


void BattleView::RenderMovementMarker(BattleRendering* rendering, Unit* unit)
{
	glm::vec2 finalDestination = unit->movement.GetFinalDestination();

	if (unit->movement.path.size() > 1 || glm::length(unit->state.center - finalDestination) > 25)
	{
		if (!unit->movement.target)
		{
			rendering->_vboTextureBillboards1._mode = GL_POINTS;
			rendering->_vboTextureBillboards1._vertices.clear();

			glm::vec3 position = GetPosition(finalDestination, 0.5);
			//float pointsize = GetUnitMarkerScreenSize(vector3(finalDestination, 0));
			glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
			glm::vec2 texcoord = texsize * glm::vec2(unit->player != _battleModel->bluePlayer ? 4 : 3, 0);

			rendering->_vboTextureBillboards1._vertices.push_back(texture_billboard_vertex(position, 32, texcoord, texsize));
			rendering->_vboTextureBillboards1.update(GL_STATIC_DRAW);

			texture_billboard_uniforms uniforms;
			uniforms._transform = GetTransform();
			uniforms._texture = rendering->_textureUnitMarkers;
			uniforms._upvector = GetCameraUpVector();
			uniforms._viewport_height = 0.25f * renderer_base::pixels_per_point() * GetViewportBounds().height();
			uniforms._min_point_size = 24 * renderer_base::pixels_per_point();
			uniforms._max_point_size = 48 * renderer_base::pixels_per_point();
			_textureBillboardRenderer->_texture_billboard_renderer->render(rendering->_vboTextureBillboards1, uniforms);
		}
	}
}


void BattleView::RenderMovementPath(BattleRendering* rendering, Unit* unit)
{
	if (unit->movement.path.size() != 0)
	{
		glm::vec2 position = unit->state.center;

		int mode = 0;
		if (unit->movement.target)
			mode = 2;
		else if (unit->movement.running)
			mode = 1;

		std::vector<glm::vec2> path(unit->movement.path);
		if (unit->movement.target != 0)
			path.insert(path.end(), unit->movement.target->state.center);

		Path(rendering->_vboMovementMarkerPath, mode, position, path, unit->movement.path_t0);

		BattleRendering::ground_texture_uniforms uniforms;
		uniforms._transform = GetTransform();
		uniforms._texture = unit->player == _battleModel->bluePlayer ? rendering->_textureMovementBlue : rendering->_textureMovementRed;
		rendering->_ground_texture_renderer->render(rendering->_vboMovementMarkerPath, uniforms);
	}
}



void BattleView::TexRectN(vertexbuffer<texture_vertex>& shape, int size, int x, int y, int w, int h)
{
	float width = w / 2.0f;
	float height = h / 2.0f;

	float xx = width / 2;
	float yy = height / 2;

	float tx1 = x / (float)size;
	float tx2 = (x + w) / (float)size;
	float ty1 = y / (float)size;
	float ty2 = (y + h) / (float)size;

	shape._mode = GL_TRIANGLE_FAN;
	shape._vertices.clear();

	shape._vertices.push_back(texture_vertex(glm::vec2(-xx, yy), glm::vec2(tx1, ty1)));
	shape._vertices.push_back(texture_vertex(glm::vec2(-xx, -yy), glm::vec2(tx1, ty2)));
	shape._vertices.push_back(texture_vertex(glm::vec2(xx, -yy), glm::vec2(tx2, ty2)));
	shape._vertices.push_back(texture_vertex(glm::vec2(xx, yy), glm::vec2(tx2, ty1)));
	shape._vertices.push_back(texture_vertex(glm::vec2(-xx, yy), glm::vec2(tx1, ty1)));
}


void BattleView::TexRect256(vertexbuffer<texture_vertex>& shape, int x, int y, int w, int h)
{
	TexRectN(shape, 256, x, y, w, h);
}


void BattleView::TexRectN(vertexbuffer<texture_vertex3>& shape, int size, int x, int y, int w, int h)
{
	float width = w / 2.0f;
	float height = h / 2.0f;

	float xx = width / 2;
	float yy = height / 2;

	float tx1 = x / (float)size;
	float tx2 = (x + w) / (float)size;
	float ty1 = y / (float)size;
	float ty2 = (y + h) / (float)size;

	shape._mode = GL_TRIANGLE_FAN;
	shape._vertices.clear();

	shape._vertices.push_back(texture_vertex3(GetPosition(glm::vec2(-xx, yy)), glm::vec2(tx1, ty1)));
	shape._vertices.push_back(texture_vertex3(GetPosition(glm::vec2(-xx, -yy)), glm::vec2(tx1, ty2)));
	shape._vertices.push_back(texture_vertex3(GetPosition(glm::vec2(xx, -yy)), glm::vec2(tx2, ty2)));
	shape._vertices.push_back(texture_vertex3(GetPosition(glm::vec2(xx, yy)), glm::vec2(tx2, ty1)));
	shape._vertices.push_back(texture_vertex3(GetPosition(glm::vec2(-xx, yy)), glm::vec2(tx1, ty1)));
}


void BattleView::TexRect256(vertexbuffer<texture_vertex3>& shape, int x, int y, int w, int h)
{
	TexRectN(shape, 256, x, y, w, h);
}


void BattleView::MissileLine(vertexbuffer<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, float scale)
{
	glm::vec2 dir = glm::normalize(p2 - p1);

	p1 += dir * 12.0f * scale;
	p2 -= dir * 12.0f * scale;

	TexLine16(shape, p1, p2, 0, 3, scale);
}


void BattleView::MissileHead(vertexbuffer<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, float scale)
{
	glm::vec2 dir = glm::normalize(p2 - p1);

	p2 -= dir * 8.0f * scale;
	p1 = p2 - dir * 8.0f * scale;

	TexLine16(shape, p1, p2, 3, 16, scale);
}


void BattleView::TexLine16(vertexbuffer<texture_vertex3>& shape, glm::vec2 p1, glm::vec2 p2, int t1, int t2, float scale)
{
	glm::vec2 diff = p2 - p1;
	glm::vec2 dir = glm::normalize(diff);

	glm::vec2 left = glm::vec2(-dir.y, dir.x);
	left *= scale * (t2 - t1) / 4;

	float n = glm::length(diff) / 8.0f / scale;

	float tx1 = t1 * (1.0f / 16.0f);
	float tx2 = t2 * (1.0f / 16.0f);

	shape._mode = GL_TRIANGLE_STRIP;
	shape._vertices.clear();

	shape._vertices.push_back(texture_vertex3(GetPosition(p2 + left), glm::vec2(tx1, 0)));
	shape._vertices.push_back(texture_vertex3(GetPosition(p1 + left), glm::vec2(tx1, n)));
	shape._vertices.push_back(texture_vertex3(GetPosition(p2 - left), glm::vec2(tx2, 0)));
	shape._vertices.push_back(texture_vertex3(GetPosition(p1 - left), glm::vec2(tx2, n)));
}


void BattleView::_Path(vertexbuffer<texture_vertex3>& shape, int mode, float scale, const std::vector<glm::vec2>& path, float t0)
{
	int t1 = 0;
	int t2 = 4;

	switch (mode)
	{
		case 1:
			t1 = 4;
			t2 = 8;
			break;

		case 2:
			t1 = 8;
			t2 = 14;
			break;
	}

	float tx1 = t1 * (1.0f / 16.0f);
	float tx2 = t2 * (1.0f / 16.0f);
	float ty1 = (t0 - glm::length(path[0] - path[1])) / scale / 8.0f;

#if TARGET_OS_IPHONE
	static float adjust = 0;
	if (adjust == 0)
	{
		if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
			adjust = 2;
		else
			adjust = 1;
	}
#else
	static float adjust = 1;
#endif

	shape._mode = GL_TRIANGLES;
	shape._vertices.clear();

	for (int i = 1; i < (int)path.size(); ++i)
	{
		glm::vec2 p1 = path[i - 1];
		glm::vec2 p2 = path[i];
		glm::vec2 d = p2 - p1;
		float length = glm::length(d);

		float ty2 = ty1 + length / scale / 8.0f;

		glm::vec2 left;

		if (length > 0.01f)
			left = adjust * scale * rotate(d / length, (float)M_PI_2);

		glm::vec2 p1L = p1 + left;
		glm::vec2 p1R = p1 - left;
		glm::vec2 p2L = p2 + left;
		glm::vec2 p2R = p2 - left;

		shape._vertices.push_back(texture_vertex3(GetPosition(p1L), glm::vec2(tx1, -ty1)));
		shape._vertices.push_back(texture_vertex3(GetPosition(p2L), glm::vec2(tx1, -ty2)));
		shape._vertices.push_back(texture_vertex3(GetPosition(p2R), glm::vec2(tx2, -ty2)));
		shape._vertices.push_back(texture_vertex3(GetPosition(p2R), glm::vec2(tx2, -ty2)));
		shape._vertices.push_back(texture_vertex3(GetPosition(p1R), glm::vec2(tx2, -ty1)));
		shape._vertices.push_back(texture_vertex3(GetPosition(p1L), glm::vec2(tx1, -ty1)));

		ty1 = ty2;
	}
}


/*static float _TrimEnd(std::vector<vector2>& path, float length)
{
	float result = 0;
	while (true)
	{
		if (path.size() < 2)
			break;

		vector2 veryLast = *(path.end() - 1);
		vector2 nextLast = *(path.end() - 2);
		float distance = norm(veryLast - nextLast);
		if (distance <= length)
		{
			if (path.size() > 2)
			{
				vector2 nextNext = *(path.end() - 3);
				result += norm(nextLast - nextNext);
			}

			path.erase(path.end() - 1);
			length -= distance;
		}
		else
		{
			*(path.end() - 1) = veryLast + (nextLast - veryLast) * length / distance;
			break;
		}
	}
	return result;
}*/



void BattleView::Path(vertexbuffer<texture_vertex3>& shape, int mode, glm::vec2 position, const std::vector<glm::vec2>& path, float t0)
{
	if (path.size() == 0)
		return;

	std::vector<glm::vec2> p;
	p.insert(p.begin(), position);
	p.insert(p.end(), path.begin(), path.end());

	//float radius = 0.0f; // movement marker radius
	//_TrimEnd(p, radius);
	//std::reverse(p.begin(), p.end());
	//t0 += _TrimEnd(p, radius);
	//std::reverse(p.begin(), p.end());

	_Path(shape, mode, 1, p, t0);
}

