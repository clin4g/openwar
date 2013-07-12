// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleView.h"
#include "UnitCounter.h"
#include "CasualtyMarker.h"
#include "SmokeCounter.h"
#include "RangeMarker.h"
#include "UnitTrackingMarker.h"
#include "UnitMovementMarker.h"
#include "ShootingCounter.h"
#include "GradientRenderer.h"
#include "ColorBillboardRenderer.h"
#include "SmoothTerrainWater.h"
#include "SmoothTerrainSky.h"
#include "PlainRenderer.h"
#include "TextureRenderer.h"
#import "sprite.h"



static affine2 billboard_texcoords(int x, int y, bool flip)
{
	float x0 = 0.125f * x;
	float x1 = x0 + 0.125f;
	float y0 = 0.125f * y;
	float y1 = y0 + 0.125f;
	return flip ? affine2(glm::vec2(x1, y0), glm::vec2(x0, y1)) : affine2(glm::vec2(x0, y0), glm::vec2(x1, y1));
}



BattleView::BattleView(Surface* screen, BattleModel* battleModel, renderers* r) : TerrainView(screen, battleModel->terrainSurface),
_renderers(r),
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
_plainTriangleRenderer(nullptr),
_gradientLineRenderer(nullptr),
_gradientTriangleRenderer(nullptr),
_gradientTriangleStripRenderer(nullptr),
_colorBillboardRenderer(nullptr),
_textureTriangleRenderer(nullptr),
_textureFacing(nullptr)
{
	_textureUnitMarkers = new texture(@"Texture256x256.png");
	_textureTouchMarker = new texture(@"TouchMarker.png");
	_textureFacing = new texture(@"Facing.png");

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
	_plainTriangleRenderer = new PlainTriangleRenderer();
	_gradientLineRenderer = new GradientLineRenderer();
	_gradientTriangleRenderer = new GradientTriangleRenderer();
	_gradientTriangleStripRenderer = new GradientTriangleStripRenderer();
	_colorBillboardRenderer = new ColorBillboardRenderer();
	_textureTriangleRenderer = new TextureTriangleRenderer();
}


BattleView::~BattleView()
{
	delete _casualtyMarker;

	for (UnitMovementMarker* marker : _movementMarkers)
		delete marker;

	for (UnitTrackingMarker* marker : _trackingMarkers)
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
	if (_terrainSurfaceRendererSmooth != nullptr)
		_terrainSurfaceRendererSmooth->InitializeTerrainShadow(GetContentBounds());

	InitializeTerrainTrees();

	InitializeCameraPosition(_battleModel->units);
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


	// Terrain Sky

	glDisable(GL_DEPTH_TEST);
	if (_battleModel->terrainSky != nullptr)
	{
		_battleModel->terrainSky->RenderBackgroundLinen(_renderers, GetViewportBounds(), GetFlip());
		_battleModel->terrainSky->Render(_renderers, GetCameraDirection().z, GetFlip());
	}


	// Terrain Surface

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (_terrainSurfaceRendererSmooth != nullptr)
		_terrainSurfaceRendererSmooth->Render(GetTransform(), _lightNormal);

	if (_terrainSurfaceRendererTiled != nullptr)
		_terrainSurfaceRendererTiled->Render(GetTransform(), _lightNormal);


	// Terrain Water

	glDisable(GL_CULL_FACE);
	if (_battleModel->terrainWater != nullptr)
		_battleModel->terrainWater->Render(GetTransform());


	// Fighter Weapons

	glDepthMask(false);
	_plainLineRenderer->Reset();
	for (UnitCounter* marker : _battleModel->_unitMarkers)
		marker->AppendFighterWeapons(_plainLineRenderer);
	_plainLineRenderer->Draw(GetTransform(), glm::vec4(0.4, 0.4, 0.4, 0.6));


	// Color Billboards

	_colorBillboardRenderer->Reset();
	_casualtyMarker->RenderCasualtyColorBillboards(_colorBillboardRenderer);
	_colorBillboardRenderer->Draw(GetTransform(), GetCameraUpVector(), GetViewportBounds().height());


	// Texture Billboards

	_billboardModel->dynamicBillboards.clear();
	_casualtyMarker->AppendCasualtyBillboards(_billboardModel);
	for (UnitCounter* marker : _battleModel->_unitMarkers)
		marker->AppendFighterBillboards(_billboardModel);
	for (SmokeCounter* marker : _battleModel->_smokeMarkers)
		marker->AppendSmokeBillboards(_billboardModel);
	_textureBillboardRenderer->Render(_billboardModel, GetTransform(), GetCameraUpVector(), glm::degrees(GetCameraFacing()), GetViewportBounds().height(), GetFlip());


	// Range Markers

	for (std::pair<int, Unit*> item : _battleModel->units)
	{
		if (item.second->player == _player)
		{
			RangeMarker marker(_battleModel, item.second);
			_gradientTriangleStripRenderer->Reset();
			marker.Render(_gradientTriangleStripRenderer);
			_gradientTriangleStripRenderer->Draw(GetTransform());
		}
	}


	// Unit Facing Markers

	glDisable(GL_DEPTH_TEST);
	_textureTriangleRenderer->Reset();

	for (UnitCounter* marker : _battleModel->_unitMarkers)
		if (marker->GetUnit()->player == _player)
			marker->AppendFacingMarker(_textureTriangleRenderer, this);
	for (UnitMovementMarker* marker : _movementMarkers)
		if (marker->GetUnit()->player == _player)
			marker->AppendFacingMarker(_textureTriangleRenderer, this);
	for (UnitTrackingMarker* marker : _trackingMarkers)
		if (marker->GetUnit()->player == _player)
			marker->AppendFacingMarker(_textureTriangleRenderer, this);

	_textureTriangleRenderer->Draw(sprite_transform(GetViewportBounds()).transform(), _textureFacing);



	// Unit Markers

	_textureBillboardRenderer1->Reset();
	_textureBillboardRenderer2->Reset();

	for (UnitCounter* marker : _battleModel->_unitMarkers)
		marker->AppendUnitMarker(_textureBillboardRenderer1, _textureBillboardRenderer2, GetFlip());
	for (UnitMovementMarker* marker : _movementMarkers)
		marker->RenderMovementMarker(_textureBillboardRenderer1);
	for (UnitTrackingMarker* marker : _trackingMarkers)
		marker->RenderTrackingMarker(_textureBillboardRenderer1);

	bounds1f sizeLimit = GetUnitIconSizeLimit();
	_textureBillboardRenderer1->Draw(_textureUnitMarkers, GetTransform(), GetCameraUpVector(), glm::degrees(GetCameraFacing()), GetViewportBounds().height(), sizeLimit);
	_textureBillboardRenderer2->Draw(_textureUnitMarkers, GetTransform(), GetCameraUpVector(), glm::degrees(GetCameraFacing()), GetViewportBounds().height(), sizeLimit);


	// Tracking Markers

	glDisable(GL_DEPTH_TEST);
	for (UnitTrackingMarker* marker : _trackingMarkers)
	{
		_textureBillboardRenderer1->Reset();
		marker->RenderTrackingShadow(_textureBillboardRenderer1);
		_textureBillboardRenderer1->Draw(_textureTouchMarker, GetTransform(), GetCameraUpVector(), glm::degrees(GetCameraFacing()), GetViewportBounds().height(), bounds1f(64, 64));
	}


	// Movement Paths

	glEnable(GL_DEPTH_TEST);
	_gradientTriangleRenderer->Reset();
	for (UnitMovementMarker* marker : _movementMarkers)
		marker->RenderMovementPath(_gradientTriangleRenderer);
	_gradientTriangleRenderer->Draw(GetTransform());//, glm::vec4(0.5, 0.5, 1, 0.25));


	// Tracking Path

	glDisable(GL_DEPTH_TEST);
	for (UnitTrackingMarker* marker : _trackingMarkers)
	{
		_gradientTriangleRenderer->Reset();
		marker->RenderTrackingPath(_gradientTriangleRenderer);
		marker->RenderOrientation(_gradientTriangleRenderer);
		_gradientTriangleRenderer->Draw(GetTransform());
	}


	// Tracking Fighters

	glEnable(GL_DEPTH_TEST);
	_colorBillboardRenderer->Reset();
	for (UnitTrackingMarker* marker : _trackingMarkers)
		marker->RenderTrackingFighters(_colorBillboardRenderer);
	_colorBillboardRenderer->Draw(GetTransform(), GetCameraUpVector(), GetViewportBounds().height());


	// Movement Fighters

	_colorBillboardRenderer->Reset();
	for (UnitMovementMarker* marker : _movementMarkers)
		marker->RenderMovementFighters(_colorBillboardRenderer);
	_colorBillboardRenderer->Draw(GetTransform(), GetCameraUpVector(), GetViewportBounds().height());


	// Shooting Counters

	_gradientLineRenderer->Reset();
	for (ShootingCounter* shootingCounter : _battleModel->_shootingCounters)
		shootingCounter->Render(_gradientLineRenderer);
	_gradientLineRenderer->Draw(GetTransform());


	// Mouse Hint

	_plainLineRenderer->Reset();
	RenderMouseHint(_plainLineRenderer);
	_plainLineRenderer->Draw(GetTransform(), glm::vec4(0, 0, 0, 0.5f));


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


UnitMovementMarker* BattleView::AddMovementMarker(Unit* unit)
{
	UnitMovementMarker* marker = new UnitMovementMarker(_battleModel, unit);
	_movementMarkers.push_back(marker);
	return marker;
}


UnitMovementMarker* BattleView::GetMovementMarker(Unit* unit)
{
	for (UnitMovementMarker* marker : _movementMarkers)
		if (marker->GetUnit() == unit)
			return marker;

	return nullptr;
}


UnitMovementMarker* BattleView::GetNearestMovementMarker(glm::vec2 position, Player player)
{
	UnitMovementMarker* result = 0;
	float nearest = INFINITY;

	for (UnitMovementMarker* marker : _movementMarkers)
	{
		Unit* unit = marker->GetUnit();
		if (player != PlayerNone && unit->player != player)
			continue;

		glm::vec2 p = unit->command.GetDestination();
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


UnitTrackingMarker* BattleView::AddTrackingMarker(Unit* unit)
{
	UnitTrackingMarker* trackingMarker = new UnitTrackingMarker(_battleModel, unit);
	_trackingMarkers.push_back(trackingMarker);
	return trackingMarker;
}


UnitTrackingMarker* BattleView::GetTrackingMarker(Unit* unit)
{
	for (UnitTrackingMarker* marker : _trackingMarkers)
		if (marker->GetUnit() == unit)
			return marker;

	return 0;
}


void BattleView::RemoveTrackingMarker(UnitTrackingMarker* trackingMarker)
{
	auto i = std::find(_trackingMarkers.begin(), _trackingMarkers.end(), trackingMarker);
	if (i != _trackingMarkers.end())
	{
		_trackingMarkers.erase(i);
		delete trackingMarker;
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


bounds2f BattleView::GetBillboardBounds(glm::vec3 position, float height)
{
	glm::mat4x4 transform = GetTransform();
	glm::vec3 upvector = GetCameraUpVector();
	float viewport_height = GetViewportBounds().height();
	bounds1f sizeLimit = GetUnitIconSizeLimit() / renderer_base::pixels_per_point();

	glm::vec3 position2 = position + height * 0.5f * viewport_height * upvector;
	glm::vec4 p = transform * glm::vec4(position, 1);
	glm::vec4 q = transform * glm::vec4(position2, 1);
	float s = glm::clamp(glm::abs(q.y / q.w - p.y / p.w), sizeLimit.min, sizeLimit.max);

	return bounds2_from_center(ViewToScreen((glm::vec2)p.xy() / p.w), s / 2);
}


bounds2f BattleView::GetUnitCurrentIconViewportBounds(Unit* unit)
{
	glm::vec3 position = GetPosition(unit->state.center, 0);
	return GetBillboardBounds(position, 32);
}


bounds2f BattleView::GetUnitFutureIconViewportBounds(Unit* unit)
{
	glm::vec3 position = GetPosition(!unit->command.path.empty() ? unit->command.path.back() : unit->state.center, 0);
	return GetBillboardBounds(position, 32);
}


bounds2f BattleView::GetUnitFacingMarkerBounds(glm::vec2 center, float direction)
{
	bounds2f iconBounds = GetBillboardBounds(GetPosition(center, 0), 32);

	glm::vec2 position = iconBounds.center();
	float size = iconBounds.height();
	float adjust = GetFlip() ? 3 * glm::half_pi<float>() : glm::half_pi<float>();

	position += 0.7f * size * vector2_from_angle(direction - GetCameraFacing() + adjust);

	return bounds2_from_center(position, 0.2f * size);
}


bounds2f BattleView::GetUnitCurrentFacingMarkerBounds(Unit* unit)
{
	return GetUnitFacingMarkerBounds(unit->state.center, unit->state.direction);
}


bounds2f BattleView::GetUnitFutureFacingMarkerBounds(Unit* unit)
{
	glm::vec2 center = !unit->command.path.empty() ? unit->command.path.back() : unit->state.center;

	return GetUnitFacingMarkerBounds(center, unit->command.facing);
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
