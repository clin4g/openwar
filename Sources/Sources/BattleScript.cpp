// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <cstring>
#include <glm/gtc/constants.hpp>

#include "BattleScript.h"
#include "BattleModel/BattleModel.h"
#include "Simulator/BattleSimulator.h"
#include "TerrainForest/BillboardTerrainForest.h"
#include "SmoothTerrain/SmoothTerrainSurface.h"
#include "TerrainSurface/TiledTerrainSurface.h"
#include "SmoothTerrain/SmoothTerrainWater.h"
#include "TerrainSky/SmoothTerrainSky.h"
#include "../Library/Renderers/GradientRenderer.h"


static BattleScript* _battlescript = nullptr;


BattleScript::BattleScript() :
_battleModel(nullptr),
_battleSimulator(nullptr),
_L(nullptr)
{
	_battleModel = new BattleModel();
	_battleModel->terrainForest = new BillboardTerrainForest();

	_battlescript = this;

	_L = luaL_newstate();
	luaL_openlibs(_L);

	lua_pushcfunction(_L, openwar_terrain_init);
	lua_setglobal(_L, "openwar_terrain_init");

	lua_pushcfunction(_L, openwar_simulator_init);
	lua_setglobal(_L, "openwar_simulator_init");


	lua_pushcfunction(_L, openwar_render_hint_line);
	lua_setglobal(_L, "openwar_render_hint_line");

	lua_pushcfunction(_L, openwar_render_hint_circle);
	lua_setglobal(_L, "openwar_render_hint_circle");


	lua_pushcfunction(_L, battle_message);
	lua_setglobal(_L, "battle_message");

	lua_pushcfunction(_L, battle_get_time);
	lua_setglobal(_L, "battle_get_time");

	lua_pushcfunction(_L, battle_new_unit);
	lua_setglobal(_L, "battle_new_unit");

	lua_pushcfunction(_L, battle_set_unit_movement);
	lua_setglobal(_L, "battle_set_unit_movement");

	lua_pushcfunction(_L, battle_get_unit_status);
	lua_setglobal(_L, "battle_get_unit_status");

	lua_pushcfunction(_L, battle_set_terrain_tile);
	lua_setglobal(_L, "battle_set_terrain_tile");

	lua_pushcfunction(_L, battle_set_terrain_height);
	lua_setglobal(_L, "battle_set_terrain_height");

	lua_pushcfunction(_L, battle_add_terrain_tree);
	lua_setglobal(_L, "battle_add_terrain_tree");
}


BattleScript::~BattleScript()
{
	lua_close(_L);

	delete _battleSimulator;

	delete _battleModel->terrainSurface;
	delete _battleModel->terrainForest;
	delete _battleModel->terrainWater;
	delete _battleModel->terrainSky;
	delete _battleModel;
}


void BattleScript::SetGlobalNumber(const char* name, double value)
{
	lua_pushnumber(_L, value);
	lua_setglobal(_L, name);
}


void BattleScript::SetGlobalString(const char* name, const char* value)
{
	lua_pushstring(_L, value);
	lua_setglobal(_L, name);
}


void BattleScript::AddStandardPath()
{
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
	NSString* path = [NSBundle mainBundle].resourcePath;
	path = [path stringByAppendingPathComponent:@"Scripts"];
	path = [path stringByAppendingPathComponent:@"?.lua"];
	AddPackagePath(path.UTF8String);
#endif
}


void BattleScript::AddPackagePath(const char* path)
{
	lua_getglobal(_L, "package");
	if (lua_isnil(_L, -1))
	{
		lua_pop(_L, 1);
		return;
	}

	lua_pushstring(_L, "path");
	lua_gettable(_L, -2);
	if (lua_isnil(_L, -1))
	{
		lua_pop(_L, 2); // package & path
		return;
	}

	std::string s(lua_tostring(_L, -1));
	lua_pop(_L, 1);

	s.append(";");
	s.append(path);

	lua_pushstring(_L, "path");
	lua_pushstring(_L, s.c_str());
	lua_settable(_L, -3);

	lua_pop(_L, 1); // package
}


void BattleScript::Execute(const char* script, size_t length)
{
	if (script != nullptr)
	{
		int error = luaL_loadbuffer(_L, script, length, "line");
		if (!error) lua_pcall(_L, 0, 0, 0);

		if (error)
		{
			//NSLog(@"BattleScript ERROR: %s", lua_tostring(_L, -1));
			lua_pop(_L, 1);  /* pop error message from the stack */
		}
	}
}


void BattleScript::Tick(double secondsSinceLastUpdate)
{
	lua_getglobal(_L, "openwar_battle_tick");

	if (lua_isnil(_L, -1))
	{
		lua_pop(_L, 1);
	}
	else
	{
		lua_pushnumber(_L, secondsSinceLastUpdate);
		int error = lua_pcall(_L, 1, 0, 0);
		if (error)
		{
			//NSLog(@"BattleScript ERROR: %s", lua_tostring(_L, -1));
			lua_pop(_L, 1);  /* pop error message from the stack */
		}
	}

	_battleSimulator->AdvanceTime((float)secondsSinceLastUpdate);
}


void BattleScript::RenderHints(GradientLineRenderer* renderer)
{
	lua_getglobal(_L, "openwar_render_hints");

	if (lua_isnil(_L, -1))
	{
		lua_pop(_L, 1);
	}
	else
	{
		_renderer = renderer;

		int error = lua_pcall(_L, 0, 0, 0);
		if (error)
		{
			//NSLog(@"BattleScript ERROR: %s", lua_tostring(_L, -1));
			lua_pop(_L, 1);  /* pop error message from the stack */
		}
	}
}


int BattleScript::NewUnit(Player player, UnitPlatform platform, UnitWeapon weapon, int strength, glm::vec2 position, float bearing)
{
	UnitStats unitStats = BattleModel::GetDefaultUnitStats(platform, weapon);

	Unit* unit = _battleModel->AddUnit(player, strength, unitStats, position);
	unit->command.facing = glm::radians(90 - bearing);

	_battleModel->AddUnitMarker(unit);

	return unit->unitId;
}


void BattleScript::SetUnitMovement(int unitId, bool running, std::vector<glm::vec2> path, int chargeId, float heading)
{
	Unit* unit = _battleModel->GetUnit(unitId);
	if (unit != nullptr)
	{
		unit->command.path = path;
		unit->command.facing = heading;
		unit->command.meleeTarget = _battleModel->GetUnit(chargeId);
		unit->command.running = running;

		//if (_battleModel->GetMovementMarker(unit) == nullptr)
		//	_battleModel->AddMovementMarker(unit);
	}
}


/***/



int BattleScript::openwar_terrain_init(lua_State* L)
{
	delete _battlescript->_battleModel->terrainSurface;
	_battlescript->_battleModel->terrainSurface = nullptr;

	int n = lua_gettop(L);
	const char* s = n < 1 ? nullptr : lua_tostring(L, 1);

	if (s != nullptr && std::strcmp(s, "smooth") == 0)
	{
		const char* p = n < 2 ? nullptr : lua_tostring(L, 2);
		const double size = n < 3 ? 1024 : lua_tonumber(L, 3);

#ifdef OPENWAR_USE_SDL

		image* map = new image(resource("Maps/DefaultMap.tiff"));
		bounds2f bounds(0, 0, size, size);

#else

		NSString* path = [NSString stringWithCString:p encoding:NSASCIIStringEncoding];
		NSData* data = [NSData dataWithContentsOfFile:path];
		image* map = ConvertTiffToImage(data);
		bounds2f bounds(0, 0, size, size);

#endif

		_battlescript->_battleModel->terrainSurface = new SmoothTerrainSurface(bounds, map);
		_battlescript->_battleModel->terrainWater = new SmoothTerrainWater(bounds, map);
		_battlescript->_battleModel->terrainSky = new SmoothTerrainSky();
	}
	else if (s != nullptr && std::strcmp(s, "tiled") == 0)
	{

		int x = n < 2 ? 0 : (int)lua_tonumber(L, 2);
		int y = n < 3 ? 0 : (int)lua_tonumber(L, 3);

		_battlescript->_battleModel->terrainSurface = new TiledTerrainSurface(bounds2f(0, 0, 1024, 1024), glm::ivec2(x, y));
	}

	return 0;
}


int BattleScript::openwar_simulator_init(lua_State* L)
{
	_battlescript->_battleSimulator = new BattleSimulator(_battlescript->_battleModel);

	return 0;
}


int BattleScript::openwar_render_hint_line(lua_State* L)
{
	int n = lua_gettop(L);

	float x1 = n < 1 ? 0 : (float)lua_tonumber(L, 1);
	float y1 = n < 2 ? 0 : (float)lua_tonumber(L, 2);
	float x2 = n < 3 ? 0 : (float)lua_tonumber(L, 3);
	float y2 = n < 4 ? 0 : (float)lua_tonumber(L, 4);

	float z1 = _battlescript->_battleModel->terrainSurface->GetHeight(glm::vec2(x1, y1));
	float z2 = _battlescript->_battleModel->terrainSurface->GetHeight(glm::vec2(x2, y2));

	glm::vec4 c(0, 0, 0, 0.5f);

	_battlescript->_renderer->AddLine(glm::vec3(x1, y1, z1), glm::vec3(x2, y2, z2), c, c);

	return 0;
}


int BattleScript::openwar_render_hint_circle(lua_State* L)
{
	int n = lua_gettop(L);

	float x = n < 1 ? 0 : (float)lua_tonumber(L, 1);
	float y = n < 2 ? 0 : (float)lua_tonumber(L, 2);
	float r = n < 3 ? 0 : (float)lua_tonumber(L, 3);

	glm::vec4 c(0, 0, 0, 0.5f);

	float da = 2 * glm::pi<float>() / 16;
	for (int i = 0; i < 16; ++i)
	{
		float a1 = i * da;
		float a2 = a1 + da;
		float x1 = x + r * glm::cos(a1);
		float y1 = y + r * glm::sin(a1);
		float x2 = x + r * glm::cos(a2);
		float y2 = y + r * glm::sin(a2);

		float z1 = _battlescript->_battleModel->terrainSurface->GetHeight(glm::vec2(x1, y1));
		float z2 = _battlescript->_battleModel->terrainSurface->GetHeight(glm::vec2(x2, y2));

		_battlescript->_renderer->AddLine(glm::vec3(x1, y1, z1), glm::vec3(x2, y2, z2), c, c);
	}

	return 0;
}


int BattleScript::battle_message(lua_State* L)
{
	int n = lua_gettop(L);

	const char* s = n < 1 ? nullptr : lua_tostring(L, 1);
	//if (s != nullptr)
	//	NSLog(@"BattleScript: %s", s);

	return 0;
}


int BattleScript::battle_get_time(lua_State* L)
{
	lua_pushnumber(L, 47.62);
	return 1;
}


int BattleScript::battle_new_unit(lua_State* L)
{
	int n = lua_gettop(L);
	Player player = n < 1 ? Player1 : ToPlayer(L, 1);
	UnitPlatform platform = n < 2 ? UnitPlatformCav : ToUnitPlatform(L, 2);
	UnitWeapon weapon = n < 3 ? UnitWeaponYari : ToUnitUnitWeapon(L, 3);
	int strength = n < 4 ? 40 : (int)lua_tonumber(L, 4);
	float x = n < 5 ? 512 : (float)lua_tonumber(L, 5);
	float y = n < 6 ? 512 : (float)lua_tonumber(L, 6);
	float b = n < 7 ? 0 :  (float)lua_tonumber(L, 7);

	int unitId = _battlescript->NewUnit(player, platform, weapon, strength, glm::vec2(x, y), b);

	lua_pushnumber(L, unitId);

	return 1;
}


int BattleScript::battle_set_unit_movement(lua_State* L)
{
	int n = lua_gettop(L);
	int unitId = n < 1 ? 0 : (int)lua_tonumber(L, 1);
	bool running = n < 2 ? false : lua_toboolean(L, 2);
	std::vector<glm::vec2> path;
	if (n >= 3) ToPath(path, L, 3);
	int chargeId = n < 4 ? 0 : (int)lua_tonumber(L, 4);
	float heading = n < 5 ? 0 : (float)lua_tonumber(L, 5);

	_battlescript->SetUnitMovement(unitId, running, path, chargeId, heading);

	return 0;
}


int BattleScript::battle_get_unit_status(lua_State* L)
{
	int n = lua_gettop(L);
	int unitId = n < 1 ? 0 : (int)lua_tonumber(L, 1);

	Unit* unit = _battlescript->_battleModel->GetUnit(unitId);
	if (unit != nullptr)
	{
		UnitStatus status(unit);
		lua_pushnumber(L, status.position.x);
		lua_pushnumber(L, status.position.y);
		lua_pushnumber(L, status.heading);
		lua_pushnumber(L, status.state);
		lua_pushnumber(L, status.strength);
		lua_pushnumber(L, status.morale);
		return 6;
	}

	return 0;
}


int BattleScript::battle_set_terrain_tile(lua_State* L)
{
	TiledTerrainSurface* terrainSurfaceModel = dynamic_cast<TiledTerrainSurface*>(_battlescript->_battleModel->terrainSurface);

	int n = lua_gettop(L);
	int x = n < 1 ? 0 : (int)lua_tonumber(L, 1);
	int y = n < 2 ? 0 : (int)lua_tonumber(L, 2);
	const char* texture = n < 3 ? nullptr : lua_tostring(L, 3);
	int rotate = n < 4 ? 0 : (int)lua_tonumber(L, 4);
	bool mirror = n < 5 ? 0 : lua_toboolean(L, 5);

	terrainSurfaceModel->SetTile(x, y, std::string(texture), rotate, mirror);

	return 0;
}


int BattleScript::battle_set_terrain_height(lua_State* L)
{
	TiledTerrainSurface* terrainSurfaceModel = dynamic_cast<TiledTerrainSurface*>(_battlescript->_battleModel->terrainSurface);

	int n = lua_gettop(L);
	int x = n < 1 ? 0 : (int)lua_tonumber(L, 1);
	int y = n < 2 ? 0 : (int)lua_tonumber(L, 2);
	float h = n < 3 ? 0 : (float)lua_tonumber(L, 3);

	terrainSurfaceModel->SetHeight(x, y, h);

	return 0;
}


int BattleScript::battle_add_terrain_tree(lua_State* L)
{
	int n = lua_gettop(L);
	float x = n < 1 ? 0 : (float)lua_tonumber(L, 1);
	float y = n < 2 ? 0 : (float)lua_tonumber(L, 2);

	_battlescript->_battleModel->terrainForest->AddTree(glm::vec2(x, y));

	return 0;
}


/***/




Player BattleScript::ToPlayer(lua_State* L, int index)
{
	return lua_tonumber(L, index) == 2 ? Player2 : Player1;
}


UnitPlatform BattleScript::ToUnitPlatform(lua_State* L, int index)
{
	const char* s = lua_tostring(L, index);
	if (s != nullptr)
	{
		if (std::strcmp(s, "CAV") == 0) return UnitPlatformCav;
		if (std::strcmp(s, "GEN") == 0) return UnitPlatformGen;
		if (std::strcmp(s, "ASH") == 0) return UnitPlatformAsh;
		if (std::strcmp(s, "SAM") == 0) return UnitPlatformSam;
	}

	//NSLog(@"BattleScript::ToUnitPlatform, unknown: <%s>", s);

	return UnitPlatformCav;
}


UnitWeapon BattleScript::ToUnitUnitWeapon(lua_State* L, int index)
{
	const char* s = lua_tostring(L, index);
	if (s != nullptr)
	{
		if (std::strcmp(s, "YARI") == 0) return UnitWeaponYari;
		if (std::strcmp(s, "KATA") == 0) return UnitWeaponKata;
		if (std::strcmp(s, "NAGI") == 0) return UnitWeaponNagi;
		if (std::strcmp(s, "BOW") == 0) return UnitWeaponBow;
		if (std::strcmp(s, "ARQ") == 0) return UnitWeaponArq;
	}

	//NSLog(@"BattleScript::ToUnitUnitWeapon, unknown: <%s>", s);

	return UnitWeaponYari;
}


void BattleScript::ToPath(std::vector<glm::vec2>& result, lua_State* L, int index)
{
	int key = 1;
	while (true)
	{
		lua_rawgeti(L, index, key);
		if (!lua_istable(L, -1))
			break;

		lua_pushstring(L, "x");
		lua_gettable(L, -2);
		float x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "y");
		lua_gettable(L, -2);
		float y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		result.push_back(glm::vec2(x, y));

		lua_pop(L, 1);

		++key;
	}
}


/***/



BattleScript::UnitStatus::UnitStatus(Unit* unit)
{
	position = unit->state.center;
	heading = 0;
	state = 0;
	strength = unit->fightersCount;
	morale = unit->state.morale;

	if (unit->state.IsRouting())
	{
		state = 3;
	}
	else if (unit->command.meleeTarget != nullptr)
	{
		state = 4;
	}
	else if (!unit->command.path.empty())
	{
		if (unit->command.running)
			state = 2;
		else
			state = 1;
	}

	// 0 = STANDING
	// 1 = WALKING
	// 2 = RUNNING
	// 3 = ROUTING
	// 4 = CHARGING
	// 5 = FIGHTING
	// 6 = SHOOTING
}
