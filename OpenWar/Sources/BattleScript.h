// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleScript_H
#define BattleScript_H

#include "SimulationState.h"
#include "lua.h"

class BattleContext;
class BattleModel;
class BattleView;
class SimulationState;
class TerrainSurfaceRendererTiled;


class BattleScript
{
	struct UnitStatus
	{
		glm::vec2 position;
		float heading;
		int state;
		int strength;
		float morale;

		UnitStatus(Unit* unit);
	};

	BattleContext* _battleContext;
	lua_State* _L;

public:
	BattleScript(BattleContext* battleContext, const char* directory, const char* script, size_t length);
	~BattleScript();

	void Tick();

private:
	int NewUnit(Player player, UnitPlatform platform, UnitWeapon weapon, int strength, glm::vec2 position, float bearing);
	void SetUnitMovement(int unitId, bool running, std::vector<glm::vec2> path, int chargeId, float heading);

	static int openwar_terrain_init(lua_State* L);
	static int openwar_simulator_init(lua_State* L);


	static int battle_message(lua_State* L);
	static int battle_get_time(lua_State* L);
	static int battle_new_unit(lua_State* L);
	static int battle_set_unit_movement(lua_State* L);
	static int battle_get_unit_status(lua_State* L);

	static int battle_set_terrain_tile(lua_State* L);
	static int battle_set_terrain_height(lua_State* L);
	static int battle_add_terrain_tree(lua_State* L);

	static Player ToPlayer(lua_State* L, int index);
	static UnitPlatform ToUnitPlatform(lua_State* L, int index);
	static UnitWeapon ToUnitUnitWeapon(lua_State* L, int index);
	static void ToPath(std::vector<glm::vec2>& result, lua_State* L, int index);
};


#endif
