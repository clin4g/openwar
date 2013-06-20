// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleView.h"
#include "BattleModel.h"
#include "TerrainSurfaceModelTiled.h"
#include "BattleContext.h"
#include "TerrainFeatureModelBillboard.h"



static affine2 billboard_texcoords(int x, int y, bool flip)
{
	float x0 = 0.125f * x;
	float x1 = x0 + 0.125f;
	float y0 = 0.125f * y;
	float y1 = y0 + 0.125f;
	return flip ? affine2(glm::vec2(x1, y0), glm::vec2(x0, y1)) : affine2(glm::vec2(x0, y0), glm::vec2(x1, y1));
}



BattleView::BattleView(Surface* screen, BattleModel* battleModel, renderers* r, BattleRendering* battleRendering, Player bluePlayer) : TerrainView(screen, battleModel->GetBattleContext()->terrainSurfaceModel),
_renderers(r),
_battleRendering(battleRendering),
_battleModel(battleModel),
_bluePlayer(bluePlayer),
_movementMarker_pathShape(),
_rangeMarker_shape(),
_missileMarker_shape(),
_trackingMarker_shadowShape(),
_trackingMarker_pathShape(),
_trackingMarker_orientationShape(),
_trackingMarker_missileHeadShape(),
_unitMarker_targetLineShape(),
_unitMarker_targetHeadShape(),
_shape_fighter_weapons(),
_terrainSurfaceRendererSmooth(nullptr),
_terrainSurfaceRendererTiled(nullptr),
_billboardTexture(nullptr),
_billboardModel(nullptr),
_billboardRenderer(nullptr)
{
	SetContentBounds(bounds2f(0, 0, 1024, 1024));

	_billboardTexture = new BillboardTexture();
	_billboardTexture->AddSheet(image(@"Billboards.png"));

	for (int i = 0; i < 8; ++i)
	{
		_billboardTreeShapes[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardTreeShapes[i], 0, billboard_texcoords(i, 0, false));

		_billboardTreeShapes[i + 8] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardTreeShapes[i + 8], 0, billboard_texcoords(i, 0, true));

		_billboardShapeCasualtyCav[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardShapeCasualtyCav[i], 0, billboard_texcoords(i, 4, false));

		_billboardShapeCasualtyCav[i + 8] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardShapeCasualtyCav[i + 8], 0, billboard_texcoords(i, 4, true));
	}

	for (int i = 0; i < 4; ++i)
	{
		_billboardShapeCasualtyAsh[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardShapeCasualtyAsh[i], 0, billboard_texcoords(i, 3, false));

		_billboardShapeCasualtyAsh[i + 4] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardShapeCasualtyAsh[i + 4], 0, billboard_texcoords(i, 3, false));

		_billboardShapeCasualtySam[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardShapeCasualtySam[i], 0, billboard_texcoords(i + 4, 3, false));

		_billboardShapeCasualtySam[i + 4] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardShapeCasualtySam[i + 4], 0, billboard_texcoords(i + 4, 3, false));
	}

	_billboardShapeFighterSamBlue = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardShapeFighterSamBlue, 0, billboard_texcoords(1, 1, false));
	_billboardTexture->SetTexCoords(_billboardShapeFighterSamBlue, 180, billboard_texcoords(2, 1, false));

	_billboardShapeFighterSamRed = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardShapeFighterSamRed, 0, billboard_texcoords(1, 2, false));
	_billboardTexture->SetTexCoords(_billboardShapeFighterSamRed, 180, billboard_texcoords(2, 2, false));

	_billboardShapeFighterAshBlue = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardShapeFighterAshBlue, 0, billboard_texcoords(3, 1, false));
	_billboardTexture->SetTexCoords(_billboardShapeFighterAshBlue, 180, billboard_texcoords(4, 1, false));

	_billboardShapeFighterAshRed = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardShapeFighterAshRed, 0, billboard_texcoords(3, 2, false));
	_billboardTexture->SetTexCoords(_billboardShapeFighterAshRed, 180, billboard_texcoords(4, 2, false));

	_billboardShapeFighterCavBlue = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavBlue,  45, billboard_texcoords(7, 1, false));
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavBlue,  90, billboard_texcoords(6, 1, false));
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavBlue, 135, billboard_texcoords(5, 1, false));
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavBlue, 225, billboard_texcoords(5, 1, true));
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavBlue, 270, billboard_texcoords(6, 1, true));
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavBlue, 315, billboard_texcoords(7, 1, true));

	_billboardShapeFighterCavRed = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavRed,  45, billboard_texcoords(7, 2, false));
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavRed,  90, billboard_texcoords(6, 2, false));
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavRed, 135, billboard_texcoords(5, 2, false));
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavRed, 225, billboard_texcoords(5, 2, true));
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavRed, 270, billboard_texcoords(6, 2, true));
	_billboardTexture->SetTexCoords(_billboardShapeFighterCavRed, 315, billboard_texcoords(7, 2, true));


	for (int i = 0; i < 8; ++i)
	{
		_billboardShapeSmoke[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardShapeSmoke[i], 0, billboard_texcoords(i, 7, false));
	}

	_billboardModel = new BillboardModel();
	_billboardModel->texture = _billboardTexture;

	_billboardRenderer = new BillboardRenderer();
}


BattleView::~BattleView()
{
}


void BattleView::OnShooting(Shooting const & shooting)
{
	_battleModel->AddShootingAndSmokeMarkers(shooting);
}


void BattleView::OnCasualty(Casualty const & casualty)
{
	_battleModel->AddCasualty(casualty);

}


static float random_float()
{
	return (float)(rand() & 0x7fff) / 0x7fff;
}


void BattleView::Initialize(SimulationState* simulationState, bool editor)
{
	InitializeTerrainShadow();
	InitializeTerrainTrees();
	InitializeTerrainWater(editor);

	InitializeCameraPosition(simulationState->units);
}


void BattleView::InitializeTerrainShadow()
{
	bounds2f bounds = GetContentBounds();
	glm::vec2 center = bounds.center();
	float radius1 = 512;
	float radius2 = 550;

	_shape_terrain_shadow._mode = GL_TRIANGLES;
	_shape_terrain_shadow._vertices.clear();

	int n = 16;
	for (int i = 0; i < n; ++i)
	{
		float angle1 = i * 2 * (float)M_PI / n;
		float angle2 = (i + 1) * 2 * (float)M_PI / n;

		glm::vec2 p1 = center + radius1 * vector2_from_angle(angle1);
		glm::vec2 p2 = center + radius2 * vector2_from_angle(angle1);
		glm::vec2 p3 = center + radius2 * vector2_from_angle(angle2);
		glm::vec2 p4 = center + radius1 * vector2_from_angle(angle2);

		_shape_terrain_shadow._vertices.push_back(plain_vertex(p1));
		_shape_terrain_shadow._vertices.push_back(plain_vertex(p2));
		_shape_terrain_shadow._vertices.push_back(plain_vertex(p3));
		_shape_terrain_shadow._vertices.push_back(plain_vertex(p3));
		_shape_terrain_shadow._vertices.push_back(plain_vertex(p4));
		_shape_terrain_shadow._vertices.push_back(plain_vertex(p1));
	}

	_shape_terrain_shadow.update(GL_STATIC_DRAW);
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
					float z = _terrainSurfaceModel->GetHeight(position);
					if (z > 0
							&& map->get_pixel((int)(position.x / 2), (int)(position.y / 2)).g > 0.5
							&& _terrainSurfaceModel->GetNormal(position).z >= 0.84)
					{
						const float adjust = 0.5 - 2.0 / 64.0; // place texture 2 texels below ground
						_billboardModel->staticBillboards.push_back(Billboard(to_vector3(position, adjust * 5), 0, 5, _billboardTreeShapes[shape]));
					}
				}

				++treeType;
			}
	}
}



static int inside_circle(glm::vec2 p)
{
	return glm::length(p - glm::vec2(512, 512)) <= 512 ? 1 : 0;
}


static int inside_circle(plain_vertex v1, plain_vertex v2, plain_vertex v3)
{
	return inside_circle(v1._position)
			+ inside_circle(v2._position)
			+ inside_circle(v3._position);

}


static vertexbuffer<plain_vertex>* choose_shape(int count, vertexbuffer<plain_vertex>* inside, vertexbuffer<plain_vertex>* border)
{
	switch (count)
	{
		case 1:
		case 2:
			return border;

		case 3:
			return inside;

		default:
			return nullptr;
	}

}

void BattleView::InitializeTerrainWater(bool editor)
{
	_shape_water_inside._mode = GL_TRIANGLES;
	_shape_water_border._mode = GL_TRIANGLES;

	_shape_water_inside._vertices.clear();
	_shape_water_border._vertices.clear();

	int n = 64;
	glm::vec2 s = glm::vec2(1024, 1024) / (float)n;
	for (int x = 0; x < n; ++x)
		for (int y = 0; y < n; ++y)
		{
			glm::vec2 p = s * glm::vec2(x, y);
			if (editor || _battleModel->GetBattleContext()->terrainSurfaceModel->ContainsWater(bounds2f(p, p + s)))
			{
				plain_vertex v11 = plain_vertex(p);
				plain_vertex v12 = plain_vertex(p + glm::vec2(0, s.y));
				plain_vertex v21 = plain_vertex(p + glm::vec2(s.x, 0));
				plain_vertex v22 = plain_vertex(p + s);

				vertexbuffer<plain_vertex>* s = choose_shape(inside_circle(v11, v22, v12), &_shape_water_inside, &_shape_water_border);
				if (s != nullptr)
				{
					s->_vertices.push_back(v11);
					s->_vertices.push_back(v22);
					s->_vertices.push_back(v12);
				}

				s = choose_shape(inside_circle(v22, v11, v21), &_shape_water_inside, &_shape_water_border);
				if (s != nullptr)
				{
					s->_vertices.push_back(v22);
					s->_vertices.push_back(v11);
					s->_vertices.push_back(v21);
				}
			}
		}

	_shape_water_inside.update(GL_STATIC_DRAW);
	_shape_water_border.update(GL_STATIC_DRAW);
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
			if (unit->player == _battleModel->_player)
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

	RenderBackgroundLinen();
	RenderBackgroundSky();

	RenderTerrainShadow();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	RenderTerrainGround();
	glDisable(GL_CULL_FACE);

	RenderTerrainWater();

	glDepthMask(false);

	RenderFighterWeapons();

	AppendCasualtyBillboards();
	AppendFighterBillboards();
	AppendSmokeBillboards();
	RenderTerrainBillboards();

	RenderRangeMarkers();
	RenderUnitMarkers();

	RenderTrackingMarkers();
	RenderMovementMarkers();

	RenderShootingMarkers();

	glDepthMask(true);

	glDisable(GL_DEPTH_TEST);
}


void BattleView::Update(double secondsSinceLastUpdate)
{
}


void BattleView::RenderBackgroundLinen()
{
	bounds2f viewport = GetViewportBounds();

	vertexbuffer<texture_vertex> shape;

	shape._mode = GL_TRIANGLES;
	shape._vertices.clear();

	glm::vec2 vt0 = glm::vec2(0, 0);
	glm::vec2 vt1 = glm::vec2((int)(viewport.width() / 128), (int)(viewport.height() / 128));

	shape._vertices.push_back(texture_vertex(glm::vec2(-1, -1), glm::vec2(vt0.x, vt0.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(-1, 1), glm::vec2(vt0.x, vt1.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(1, 1), glm::vec2(vt1.x, vt1.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(1, 1), glm::vec2(vt1.x, vt1.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(1, -1), glm::vec2(vt1.x, vt0.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(-1, -1), glm::vec2(vt0.x, vt0.y)));

	texture_uniforms uniforms;
	uniforms._transform = GetFlip() ? glm::scale(glm::mat4x4(), glm::vec3(-1.0f, -1.0f, 1.0f)) : glm::mat4x4();
	uniforms._texture = _battleRendering->_textureBackgroundLinen;

	_renderers->_texture_renderer->render(shape, uniforms);
}


void BattleView::RenderTerrainShadow()
{
	plain_uniforms uniforms;
	uniforms._transform = GetTransform();

	_battleRendering->_ground_shadow_renderer->render(_shape_terrain_shadow, uniforms);
}


void BattleView::RenderBackgroundSky()
{
	vertexbuffer<color_vertex> shape;

	float y = GetCameraDirection().z;
	float x = sqrtf(1 - y * y);
	float a = 1 - fabsf(atan2f(y, x) / (float)M_PI_2);
	float blend = bounds1f(0, 1).clamp(3.0f * (a - 0.3f));

	shape._mode = GL_TRIANGLES;
	shape._vertices.clear();

	glm::vec4 c1 = glm::vec4(56, 165, 230, 0) / 255.0f;
	glm::vec4 c2 = glm::vec4(160, 207, 243, 255) / 255.0f;
	c2.a = blend;

	shape._vertices.push_back(color_vertex(glm::vec2(-1, 0.2), c1));
	shape._vertices.push_back(color_vertex(glm::vec2(-1, 1.0), c2));
	shape._vertices.push_back(color_vertex(glm::vec2(1, 1.0), c2));
	shape._vertices.push_back(color_vertex(glm::vec2(1, 1.0), c2));
	shape._vertices.push_back(color_vertex(glm::vec2(1, 0.2), c1));
	shape._vertices.push_back(color_vertex(glm::vec2(-1, 0.2), c1));

	gradient_uniforms uniforms;
	uniforms._transform = GetFlip() ? glm::scale(glm::mat4x4(), glm::vec3(-1.0f, -1.0f, 1.0f)) : glm::mat4x4();

	_renderers->_gradient_renderer->render(shape, uniforms);
}


void BattleView::RenderTerrainGround()
{
	if (_terrainSurfaceRendererSmooth != nullptr)
		_terrainSurfaceRendererSmooth->Render(GetTransform(), _lightNormal);

	if (_terrainSurfaceRendererTiled != nullptr)
		_terrainSurfaceRendererTiled->Render(GetTransform(), _lightNormal);
}


void BattleView::RenderTerrainWater()
{
	BattleRendering::ground_texture_uniforms uniforms;
	uniforms._transform = GetTransform();
	uniforms._texture = nullptr;

	_battleRendering->_water_inside_renderer->render(_shape_water_inside, uniforms);
	_battleRendering->_water_border_renderer->render(_shape_water_border, uniforms);
}


void BattleView::RenderFighterWeapons()
{
	_shape_fighter_weapons._mode = GL_LINES;
	_shape_fighter_weapons._vertices.clear();

	for (UnitMarker* marker : _battleModel->_unitMarkers)
	{
		AppendFighterWeapons(marker->_unit);
	}

	BattleRendering::ground_color_uniforms uniforms3;
	uniforms3._transform = GetTransform();
	uniforms3._color = glm::vec4(0.4, 0.4, 0.4, 0.6);
	_battleRendering->_ground_plain_renderer->render(_shape_fighter_weapons, uniforms3);

}


void BattleView::AppendFighterWeapons(Unit* unit)
{
	if (unit->stats.weaponReach > 0)
	{
		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 p1 = fighter->state.position;
			glm::vec2 p2 = p1 + unit->stats.weaponReach * vector2_from_angle(fighter->state.direction);

			_shape_fighter_weapons._vertices.push_back(plain_vertex3(to_vector3(p1)));
			_shape_fighter_weapons._vertices.push_back(plain_vertex3(to_vector3(p2)));
		}
	}
}


void BattleView::AppendCasualtyBillboards()
{
	if (_battleModel->_casualtyMarker->casualties.empty())
		return;

	_color_billboards._mode = GL_POINTS;
	_color_billboards._vertices.clear();

	glm::vec4 c1 = glm::vec4(1, 1, 1, 0.8);
	glm::vec4 cr = glm::vec4(1, 0, 0, 0);
	glm::vec4 cb = glm::vec4(0, 0, 1, 0);
	for (const CasualtyMarker::Casualty& casualty : _battleModel->_casualtyMarker->casualties)
	{
		if (casualty.time <= 1)
		{
			glm::vec4 c = glm::mix(c1, casualty.player == Player1 ? cb : cr, casualty.time);
			_color_billboards._vertices.push_back(BattleRendering::color_billboard_vertex(casualty.position, c, 6.0));
		}

		int shape = 0;
		float height = 0;
		//int j = 0, i = 0;
		switch (casualty.platform)
		{
			case UnitPlatformAsh:
				shape = _billboardShapeCasualtyAsh[casualty.seed & 7];
				height = 2.5f;
			    //i = 3;
				//j = casualty.seed & 3;
				break;
			case UnitPlatformSam:
				shape = _billboardShapeCasualtySam[casualty.seed & 7];
				height = 2.5f;
				//i = 3;
				//j = 4 + (casualty.seed & 3);
				break;
			case UnitPlatformCav:
			case UnitPlatformGen:
				shape = _billboardShapeCasualtySam[casualty.seed & 15];
				height = 3.0f;
				//i = 4;
				//j = casualty.seed & 7;
				break;
		}

		const float adjust = 0.5 - 2.0 / 64.0; // place texture 2 texels below ground
		glm::vec3 p = to_vector3(casualty.position.xy(), adjust * height);
		_billboardModel->dynamicBillboards.push_back(Billboard(p, 0, height, shape));

	}

	BattleRendering::color_billboard_uniforms uniforms;
	uniforms._transform = GetTransform();
	uniforms._upvector = GetCameraUpVector();
	uniforms._viewport_height = 0.25f * renderer_base::pixels_per_point() * GetViewportBounds().height();

	_battleRendering->_color_billboard_renderer->render(_color_billboards, uniforms);
}


void BattleView::AppendFighterBillboards()
{
	for (UnitMarker* marker : _battleModel->_unitMarkers)
	{
		Unit* unit = marker->_unit;

		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			float size = 2.0;
			//float diff = angle_difference(GetCameraFacing(), fighter->state.direction);
			//float absdiff = fabsf(diff);

			int shape = 0;
			//int i = unit->player == Player2 ? 2 : 1;
			//int j = 0;
			switch (unit->stats.unitPlatform)
			{
				case UnitPlatformCav:
				case UnitPlatformGen:
					shape = unit->player == Player2 ? _billboardShapeFighterCavRed : _billboardShapeFighterCavBlue;
					size = 3.0;
					/*if (absdiff < 0.33 * M_PI)
						j = 5;
					else if (absdiff < 0.66 * M_PI)
						j = 6;
					else
						j = 7;*/
					break;

				case UnitPlatformSam:
					shape = unit->player == Player2 ? _billboardShapeFighterSamRed : _billboardShapeFighterSamBlue;
					size = 2.0;
					//j = absdiff < M_PI_2 ? 2 : 1;
					break;

				case UnitPlatformAsh:
					shape = unit->player == Player2 ? _billboardShapeFighterAshRed : _billboardShapeFighterAshBlue;
					size = 2.0;
					//j = absdiff < M_PI_2 ? 4 : 3;
					break;
			}


			const float adjust = 0.5 - 2.0 / 64.0; // place texture 2 texels below ground
			glm::vec3 p = to_vector3(fighter->state.position, adjust * size);
			float facing = glm::degrees(fighter->state.direction);
			_billboardModel->dynamicBillboards.push_back(Billboard(p, facing, size, shape));
		}
	}
}


void BattleView::AppendSmokeBillboards()
{
	for (SmokeMarker* marker : _battleModel->_smokeMarkers)
	{
		for (SmokeMarker::Particle& projectile : marker->particles)
		{
			if (projectile.time > 0)
			{
				int i = (int)(8 * projectile.time);
				if (i > 7)
					i = 7;

				_billboardModel->dynamicBillboards.push_back(Billboard(projectile.position, 0, 1 + 3 * projectile.time, _billboardShapeSmoke[i]));
			}
		}
	}
}


void BattleView::RenderTerrainBillboards()
{
	_billboardRenderer->Render(_billboardModel, GetTransform(), GetCameraUpVector(), glm::degrees(GetCameraFacing()));
}


void BattleView::RenderRangeMarkers()
{
	for (RangeMarker* marker : _battleModel->_rangeMarkers)
	{
		Unit* unit = marker->_unit;
		if (unit->stats.maximumRange > 0 && unit->state.unitMode != UnitModeMoving && !unit->state.IsRouting())
		{
			BattleView::MakeRangeMarker(_rangeMarker_shape, unit->state.center, unit->state.direction, 20, unit->stats.maximumRange);

			BattleRendering::ground_gradient_uniforms uniforms;
			uniforms._transform = GetTransform();

			_battleRendering->_ground_gradient_renderer->render(_rangeMarker_shape, uniforms);
		}
	}
}


void BattleView::MakeRangeMarker(vertexbuffer<color_vertex3>& shape, glm::vec2 position, float direction, float minimumRange, float maximumRange)
{
	const float thickness = 8;
	const float two_pi = 2 * (float)M_PI;
	glm::vec4 c0 = glm::vec4(255, 64, 64, 0) / 255.0f;
	glm::vec4 c1 = glm::vec4(255, 64, 64, 16) / 255.0f;

	shape._mode = GL_TRIANGLE_STRIP;
	shape._vertices.clear();

	float d = direction - two_pi / 8;
	glm::vec2 p2 = maximumRange * vector2_from_angle(d - 0.03f);
	glm::vec2 p3 = minimumRange * vector2_from_angle(d);
	glm::vec2 p4 = maximumRange * vector2_from_angle(d);
	glm::vec2 p5 = (maximumRange - thickness) * vector2_from_angle(d);
	glm::vec2 p1 = p3 + (p2 - p4);

	for (int i = 0; i <= 8; ++i)
	{
		float t = i / 8.0f;
		shape._vertices.push_back(color_vertex3(to_vector3(position + glm::mix(p3, p5, t)), c0));
		shape._vertices.push_back(color_vertex3(to_vector3(position + glm::mix(p1, p2, t)), c1));
	}

	shape._vertices.push_back(color_vertex3(to_vector3(position + p4), c1));
	shape._vertices.push_back(color_vertex3(to_vector3(position + p4), c1));
	shape._vertices.push_back(color_vertex3(to_vector3(position + p5), c0));

	int n = 10;
	for (int i = 0; i <= n; ++i)
	{
		float k = (i - (float)n / 2) / n;
		d = direction + k * two_pi / 4;
		shape._vertices.push_back(color_vertex3(to_vector3(position + (maximumRange - thickness) * vector2_from_angle(d)), c0));
		shape._vertices.push_back(color_vertex3(to_vector3(position + maximumRange * vector2_from_angle(d)), c1));
	}

	d = direction + two_pi / 8;
	p2 = maximumRange * vector2_from_angle(d + 0.03f);
	p3 = minimumRange * vector2_from_angle(d);
	p4 = maximumRange * vector2_from_angle(d);
	p5 = (maximumRange - thickness) * vector2_from_angle(d);
	p1 = p3 + (p2 - p4);

	shape._vertices.push_back(color_vertex3(to_vector3(position + p4), c1));
	for (int i = 0; i <= 8; ++i)
	{
		float t = i / 8.0f;
		shape._vertices.push_back(color_vertex3(to_vector3(position + glm::mix(p2, p1, t)), c1));
		shape._vertices.push_back(color_vertex3(to_vector3(position + glm::mix(p5, p3, t)), c0));
	}
}


void BattleView::RenderUnitMarkers()
{
	_color_billboards._mode = GL_POINTS;
	_color_billboards._vertices.clear();

	_texture_billboards1._mode = GL_POINTS;
	_texture_billboards1._vertices.clear();

	_texture_billboards2._mode = GL_POINTS;
	_texture_billboards2._vertices.clear();

	for (UnitMarker* marker : _battleModel->_unitMarkers)
	{
		Unit* unit = marker->_unit;

		RenderUnitMissileTarget(unit);
		AppendUnitMarker(marker);
	}

	BattleRendering::color_billboard_uniforms uniforms1;
	uniforms1._transform = GetTransform();
	uniforms1._upvector = GetCameraUpVector();
	uniforms1._viewport_height = 0.25f * renderer_base::pixels_per_point() * GetViewportBounds().height();
	_battleRendering->_color_billboard_renderer->render(_color_billboards, uniforms1);

	float y = GetCameraDirection().z;
	float x = sqrtf(1 - y * y);
	float a = 1 - fabsf(atan2f(y, x) / (float)M_PI_2);
	static bool* is_iphone = nullptr;
	if (is_iphone == nullptr)
	{
#if TARGET_OS_IPHONE
		is_iphone = new bool([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone);
#else
		is_iphone = new bool(false);
#endif
	}

	texture_billboard_uniforms uniforms2;
	uniforms2._transform = GetTransform();
	uniforms2._texture = _battleRendering->_textureUnitMarkers;
	uniforms2._upvector = GetCameraUpVector();
	uniforms2._viewport_height = 0.25f * renderer_base::pixels_per_point() * GetViewportBounds().height();
	uniforms2._min_point_size = (32 - 8 * a) * renderer_base::pixels_per_point();
	uniforms2._max_point_size = (32 + 16 * a) * renderer_base::pixels_per_point();
	if (*is_iphone)
	{
		uniforms2._min_point_size *= 57.0f / 72.0f;
		uniforms2._max_point_size *= 57.0f / 72.0f;
	}

	_texture_billboards1.update(GL_STATIC_DRAW);

	glDisable(GL_DEPTH_TEST);
	_billboardRenderer->_texture_billboard_renderer->render(_texture_billboards1, uniforms2);
	_billboardRenderer->_texture_billboard_renderer->render(_texture_billboards2, uniforms2);
	glEnable(GL_DEPTH_TEST);
}


void BattleView::AppendUnitMarker(UnitMarker* marker)
{
	Unit* unit = marker->_unit;

	bool routingIndicator = false;
	float routingBlinkTime = unit->state.GetRoutingBlinkTime();

	if (unit->state.IsRouting())
	{
		routingIndicator = true;
	}
	else if (routingBlinkTime != 0 && 0 <= marker->_routingTimer && marker->_routingTimer < 0.2f)
	{
		routingIndicator = true;
	}

	int state = 0;
	if (routingIndicator)
		state = 2;
	else if (unit->player != _bluePlayer)
		state = 1;

	glm::vec3 position = to_vector3(unit->state.center, 0);
	glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
	glm::vec2 texcoord1 = texsize * glm::vec2(state, 0);
	glm::vec2 texcoord2 = texsize * glm::vec2((int)unit->stats.unitWeapon, 1 + (int)unit->stats.unitPlatform);

	if (GetFlip())
	{
		texcoord1 += texsize;
		texcoord2 += texsize;
		texsize = -texsize;
	}

	_texture_billboards1._vertices.push_back(texture_billboard_vertex(position, 32, texcoord1, texsize));
	_texture_billboards2._vertices.push_back(texture_billboard_vertex(position, 32, texcoord2, texsize));
}


void BattleView::RenderUnitMissileTarget(Unit* unit)
{
	float scale = 0.5;

	if (unit->missileTarget != nullptr && unit->missileTargetLocked)
	{
		BattleView::MissileLine(_unitMarker_targetLineShape, unit->state.center, unit->missileTarget->state.center, scale);
		BattleView::MissileHead(_unitMarker_targetHeadShape, unit->state.center, unit->missileTarget->state.center, scale);

		BattleRendering::ground_texture_uniforms uniforms;
		uniforms._transform = GetTransform();
		uniforms._texture = unit->player == _bluePlayer ? _battleRendering->_textureMissileBlue : _battleRendering->_textureMissileRed;;

		_battleRendering->_ground_texture_renderer->render(_unitMarker_targetLineShape, uniforms);
		_battleRendering->_ground_texture_renderer->render(_unitMarker_targetHeadShape, uniforms);
	}
}


void BattleView::RenderTrackingMarkers()
{
	for (TrackingMarker* marker : _battleModel->_trackingMarkers)
	{
		glDisable(GL_DEPTH_TEST);
		RenderTrackingMarker(marker);
		RenderTrackingShadow(marker);
		glEnable(GL_DEPTH_TEST);
		RenderTrackingPath(marker);
		RenderTrackingOrientation(marker);
		RenderTrackingFighters(marker);
	}
}


static glm::vec2 DestinationXXX(TrackingMarker* marker)
{
	return marker->_destinationUnit ? marker->_destinationUnit->state.center
			: marker->_path.size() != 0 ? *(marker->_path.end() - 1)
					: marker->_hasDestination ? marker->_destination
							: marker->_unit->state.center;
}


void BattleView::RenderTrackingMarker(TrackingMarker* marker)
{
	glm::vec2 position = marker->_unit->state.center;
	glm::vec2 destination = DestinationXXX(marker);

	if (marker->_destinationUnit || marker->_hasDestination)
	{
		if (glm::length(position - destination) > 25)
		{
			//BoardView::WalkMarker(_movementShape, _texture, position, destination, mode, scale);
			//_movementSprite->SetVisible(true);
		}
		else
		{
			//_movementSprite->SetVisible(false);
		}

		if (marker->_destinationUnit == nullptr)
		{
			_texture_billboards1._mode = GL_POINTS;
			_texture_billboards1._vertices.clear();

			glm::vec3 position = to_vector3(destination, 0);
			//float pointsize = GetUnitMarkerScreenSize(position);
			glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
			glm::vec2 texcoord = texsize * glm::vec2(marker->_unit->player != _bluePlayer ? 4 : 3, 0);

			_texture_billboards1._vertices.push_back(texture_billboard_vertex(position, 32, texcoord, texsize));
			_texture_billboards1.update(GL_STATIC_DRAW);

			texture_billboard_uniforms uniforms;
			uniforms._transform = GetTransform();
			uniforms._texture = _battleRendering->_textureUnitMarkers;
			uniforms._upvector = GetCameraUpVector();
			uniforms._viewport_height = 0.25f * renderer_base::pixels_per_point() * GetViewportBounds().height();
			uniforms._min_point_size = 24 * renderer_base::pixels_per_point();
			uniforms._max_point_size = 48 * renderer_base::pixels_per_point();
			_billboardRenderer->_texture_billboard_renderer->render(_texture_billboards1, uniforms);
		}
	}
}


void BattleView::RenderTrackingShadow(TrackingMarker* marker)
{
	glm::vec2 p = DestinationXXX(marker);
	glm::vec2 offset = ContentToScreen(to_vector3(p, 0));
	float scale = 5;

	TexRectN(_trackingMarker_shadowShape, 32, 0, 0, 32, 32);

	for (texture_vertex& vertex : _trackingMarker_shadowShape._vertices)
	{
		vertex._position *= scale;
		vertex._position += offset;
	}

	sprite_transform s;
	s._viewport = GetViewportBounds();

	texture_uniforms uniforms;
	uniforms._transform = s.transform();//GetTransform() * matrix4::translate(p._x, p._y, 0) * matrix4::scale(scale, scale, 1);
	uniforms._texture = _battleRendering->_textureTouchMarker;
	_renderers->_texture_renderer->render(_trackingMarker_shadowShape, uniforms);
}


void BattleView::RenderTrackingPath(TrackingMarker* marker)
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

		BattleView::Path(_trackingMarker_pathShape, mode, position, path, 0);

		BattleRendering::ground_texture_uniforms uniforms;
		uniforms._transform = GetTransform();
		uniforms._texture = _battleRendering->_textureMovementGray;
		_battleRendering->_ground_texture_renderer->render(_trackingMarker_pathShape, uniforms);
	}
}


void BattleView::RenderTrackingOrientation(TrackingMarker* marker)
{
	if (marker->_orientationUnit != nullptr || marker->_hasOrientation)
	{
		glm::vec2 destination = DestinationXXX(marker);
		glm::vec2 orientation = marker->_orientationUnit ? marker->_orientationUnit->state.center : marker->_orientation;

		BattleView::MissileLine(_trackingMarker_orientationShape, destination, orientation, 0.5);
		BattleView::MissileHead(_trackingMarker_missileHeadShape, destination, orientation, 0.5);

		BattleRendering::ground_texture_uniforms uniforms;
		uniforms._transform = GetTransform();
		uniforms._texture = _battleRendering->_textureMissileGray;

		//_ground_texture_renderer3->render(_trackingMarker_orientationShape, uniforms);

		if (marker->_orientationUnit != nullptr)
			_battleRendering->_ground_texture_renderer->render(_trackingMarker_missileHeadShape, uniforms);
	}
}


void BattleView::RenderTrackingFighters(TrackingMarker* marker)
{
	if (!marker->_destinationUnit && !marker->_orientationUnit)
	{
		bool isBlue = marker->_unit->player == _bluePlayer;
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 16) / 255.0f : glm::vec4(255, 0, 0, 16) / 255.0f;

		glm::vec2 destination = DestinationXXX(marker);
		glm::vec2 orientation = marker->_orientationUnit ? marker->_orientationUnit->state.center : marker->_orientation;

		Formation formation = marker->_unit->formation;
		formation.SetDirection(angle(orientation - destination));

		glm::vec2 frontLeft = formation.GetFrontLeft(destination);

		_color_billboards._mode = GL_POINTS;
		_color_billboards._vertices.clear();

		for (Fighter* fighter = marker->_unit->fighters, * end = fighter + marker->_unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 offsetRight = formation.towardRight * (float)Unit::GetFighterFile(fighter);
			glm::vec2 offsetBack = formation.towardBack * (float)Unit::GetFighterRank(fighter);

			_color_billboards._vertices.push_back(BattleRendering::color_billboard_vertex(to_vector3(frontLeft + offsetRight + offsetBack, 0.5), color, 3.0));
		}

		BattleRendering::color_billboard_uniforms uniforms;
		uniforms._transform = GetTransform();
		uniforms._upvector = GetCameraUpVector();
		uniforms._viewport_height = 0.25f * renderer_base::pixels_per_point() * GetViewportBounds().height();

		_battleRendering->_color_billboard_renderer->render(_color_billboards, uniforms);
	}
}


void BattleView::RenderMovementMarkers()
{
	for (MovementMarker* marker : _battleModel->_movementMarkers)
	{
		glDisable(GL_DEPTH_TEST);
		RenderMovementMarker(marker->_unit);
		glEnable(GL_DEPTH_TEST);
		RenderMovementPath(marker->_unit);
		RenderMovementFighters(marker->_unit);
	}
}


void BattleView::RenderMovementMarker(Unit* unit)
{
	glm::vec2 finalDestination = unit->movement.GetFinalDestination();

	if (unit->movement.path.size() > 1 || glm::length(unit->state.center - finalDestination) > 25)
	{
		if (!unit->movement.target)
		{
			_texture_billboards1._mode = GL_POINTS;
			_texture_billboards1._vertices.clear();

			glm::vec3 position = to_vector3(finalDestination, 0.5);
			//float pointsize = GetUnitMarkerScreenSize(vector3(finalDestination, 0));
			glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
			glm::vec2 texcoord = texsize * glm::vec2(unit->player != _bluePlayer ? 4 : 3, 0);

			_texture_billboards1._vertices.push_back(texture_billboard_vertex(position, 32, texcoord, texsize));
			_texture_billboards1.update(GL_STATIC_DRAW);

			texture_billboard_uniforms uniforms;
			uniforms._transform = GetTransform();
			uniforms._texture = _battleRendering->_textureUnitMarkers;
			uniforms._upvector = GetCameraUpVector();
			uniforms._viewport_height = 0.25f * renderer_base::pixels_per_point() * GetViewportBounds().height();
			uniforms._min_point_size = 24 * renderer_base::pixels_per_point();
			uniforms._max_point_size = 48 * renderer_base::pixels_per_point();
			_billboardRenderer->_texture_billboard_renderer->render(_texture_billboards1, uniforms);
		}
	}
}


void BattleView::RenderMovementPath(Unit* unit)
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

		Path(_movementMarker_pathShape, mode, position, path, unit->movement.path_t0);

		BattleRendering::ground_texture_uniforms uniforms;
		uniforms._transform = GetTransform();
		uniforms._texture = unit->player == _bluePlayer ? _battleRendering->_textureMovementBlue : _battleRendering->_textureMovementRed;
		_battleRendering->_ground_texture_renderer->render(_movementMarker_pathShape, uniforms);
	}
}


void BattleView::RenderMovementFighters(Unit* unit)
{
	if (!unit->movement.target)
	{
		bool isBlue = unit->player == _bluePlayer;
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 32) / 255.0f : glm::vec4(255, 0, 0, 32) / 255.0f;

		glm::vec2 finalDestination = unit->movement.GetFinalDestination();

		Formation formation = unit->formation;
		formation.SetDirection(unit->movement.direction);

		glm::vec2 frontLeft = formation.GetFrontLeft(finalDestination);

		_color_billboards._mode = GL_POINTS;
		_color_billboards._vertices.clear();

		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 offsetRight = formation.towardRight * (float)Unit::GetFighterFile(fighter);
			glm::vec2 offsetBack = formation.towardBack * (float)Unit::GetFighterRank(fighter);

			_color_billboards._vertices.push_back(BattleRendering::color_billboard_vertex(to_vector3(frontLeft + offsetRight + offsetBack, 0.5), color, 3.0));
		}

		BattleRendering::color_billboard_uniforms uniforms;
		uniforms._transform = GetTransform();
		uniforms._upvector = GetCameraUpVector();
		uniforms._viewport_height = 0.25f * renderer_base::pixels_per_point() * GetViewportBounds().height();

		_battleRendering->_color_billboard_renderer->render(_color_billboards, uniforms);
	}
}


void BattleView::RenderShootingMarkers()
{
	_missileMarker_shape._mode = GL_LINES;
	_missileMarker_shape._vertices.clear();

	for (ShootingMarker* marker : _battleModel->_shootingMarkers)
	{
		AppendShootingMarker(marker);
	}

	glLineWidth(1);
	gradient_uniforms uniforms;
	uniforms._transform = GetTransform();
	_renderers->_gradient_renderer3->render(_missileMarker_shape, uniforms);
}


void BattleView::AppendShootingMarker(ShootingMarker* marker)
{
	for (ShootingMarker::Projectile& projectile : marker->_projectiles)
	{
		float t = projectile.time / projectile.duration;
		if (0 <= t && t <= 1)
		{
			if (marker->_unitWeapon == UnitWeaponArq)
				AppendShootingMarkerBullet(projectile.position1, projectile.position2, t);
			else
				AppendShootingMarkerArrow(projectile.position1, projectile.position2, t);
		}
	}
}


void BattleView::AppendShootingMarkerArrow(glm::vec3 p1, glm::vec3 p2, float t)
{
	float size = 4;
	glm::vec3 diff = p2 - p1;
	float distance = glm::length(diff);
	glm::vec3 dir = diff / distance;

	float dt = size / distance;
	float t1 = t * (1 - dt);
	float t2 = t1 + dt;
	float h1 = 0.3f * distance * (t1 - t1 * t1);
	float h2 = 0.3f * distance * (t2 - t2 * t2);

	glm::vec3 pp1 = p1 + t * (distance - size) * dir;
	glm::vec3 pp2 = pp1 + size * dir;

	pp1.z += h1;
	pp2.z += h2;

	_missileMarker_shape._vertices.push_back(color_vertex3(pp1, glm::vec4(0, 0, 0, 0.2)));
	_missileMarker_shape._vertices.push_back(color_vertex3(pp2, glm::vec4(0, 0, 0, 0.2)));
}


void BattleView::AppendShootingMarkerBullet(glm::vec3 p1, glm::vec3 p2, float t)
{
	float size = 50;
	glm::vec3 diff = p2 - p1;
	glm::vec3 dir = glm::normalize(diff);

	glm::vec3 pp1 = p1 + t * (glm::length(diff) - size) * dir;
	glm::vec3 pp2 = pp1 + size * dir;

	_missileMarker_shape._vertices.push_back(color_vertex3(pp1, glm::vec4(1, 1, 0, 0.2)));
	_missileMarker_shape._vertices.push_back(color_vertex3(pp2, glm::vec4(1, 1, 1, 0.2)));
}


texture_billboard_vertex BattleView::MakeBillboardVertex(glm::vec2 position, float height, int i, int j, bool flipx, bool flipy)
{
	glm::vec2 texsize = glm::vec2(0.125, 0.125);
	glm::vec2 texcoord = texsize * glm::vec2(j, i);

	if (flipx)
	{
		texcoord.x += texsize.x;
		texsize.x = -texsize.x;
	}
	if (flipy)
	{
		texcoord.y += texsize.y;
		texsize.y = -texsize.y;
	}

	const float adjust = 0.5 - 2.0 / 64.0; // place texture 2 texels below ground
	glm::vec3 p = to_vector3(position, adjust * height);
	return texture_billboard_vertex(p, height, texcoord, texsize);
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

	shape._vertices.push_back(texture_vertex3(to_vector3(glm::vec2(-xx, yy)), glm::vec2(tx1, ty1)));
	shape._vertices.push_back(texture_vertex3(to_vector3(glm::vec2(-xx, -yy)), glm::vec2(tx1, ty2)));
	shape._vertices.push_back(texture_vertex3(to_vector3(glm::vec2(xx, -yy)), glm::vec2(tx2, ty2)));
	shape._vertices.push_back(texture_vertex3(to_vector3(glm::vec2(xx, yy)), glm::vec2(tx2, ty1)));
	shape._vertices.push_back(texture_vertex3(to_vector3(glm::vec2(-xx, yy)), glm::vec2(tx1, ty1)));
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

	shape._vertices.push_back(texture_vertex3(to_vector3(p2 + left), glm::vec2(tx1, 0)));
	shape._vertices.push_back(texture_vertex3(to_vector3(p1 + left), glm::vec2(tx1, n)));
	shape._vertices.push_back(texture_vertex3(to_vector3(p2 - left), glm::vec2(tx2, 0)));
	shape._vertices.push_back(texture_vertex3(to_vector3(p1 - left), glm::vec2(tx2, n)));
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

		shape._vertices.push_back(texture_vertex3(to_vector3(p1L), glm::vec2(tx1, -ty1)));
		shape._vertices.push_back(texture_vertex3(to_vector3(p2L), glm::vec2(tx1, -ty2)));
		shape._vertices.push_back(texture_vertex3(to_vector3(p2R), glm::vec2(tx2, -ty2)));
		shape._vertices.push_back(texture_vertex3(to_vector3(p2R), glm::vec2(tx2, -ty2)));
		shape._vertices.push_back(texture_vertex3(to_vector3(p1R), glm::vec2(tx2, -ty1)));
		shape._vertices.push_back(texture_vertex3(to_vector3(p1L), glm::vec2(tx1, -ty1)));

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

