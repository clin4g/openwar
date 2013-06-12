// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleScript_H
#define BattleScript_H

#include "SimulationState.h"
#include "lua.h"


class BattleModel;
class SimulationState;


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

	BattleModel* _battleModel;
	SimulationState* _simulationState;
	lua_State* _L;

public:
	BattleScript(BattleModel* battleModel, SimulationState* simulationState);
	~BattleScript();

	void Tick();

private:
	int NewUnit(Player player, UnitPlatform platform, UnitWeapon weapon, int strength, glm::vec2 position, float heading);
	void SetUnitMovement(int unitId, bool running, std::vector<glm::vec2> path, int chargeId, float heading);

	static int battle_message(lua_State* L);
	static int battle_get_time(lua_State* L);
	static int battle_new_unit(lua_State* L);
	static int battle_set_unit_movement(lua_State* L);
	static int battle_get_unit_status(lua_State* L);

	static Player ToPlayer(lua_State* L, int index);
	static UnitPlatform ToUnitPlatform(lua_State* L, int index);
	static UnitWeapon ToUnitUnitWeapon(lua_State* L, int index);
	static void ToPath(std::vector<glm::vec2>& result, lua_State* L, int index);
};


#endif
