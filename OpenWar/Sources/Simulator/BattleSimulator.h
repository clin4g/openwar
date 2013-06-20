// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SIMULATIONRULES_H
#define SIMULATIONRULES_H

#include "BattleModel.h"
#include "quadtree.h"

class Fighter;
class Unit;
class BattleView;


class SimulationListener
{
public:
	virtual ~SimulationListener();
	virtual void OnShooting(const Shooting& shooting) = 0;
	virtual void OnCasualty(const Casualty& casualty) = 0;
	virtual BattleModel* GetBattleModel() const = 0;
	virtual BattleView* GetBattleView() = 0;
};


class BattleSimulator
{
	BattleModel* _battleModel;
	quadtree<Fighter*> _weaponQuadTree;
	quadtree<Fighter*> _fighterQuadTree;
	float _secondsSinceLastTimeStep;

public:
	Player currentPlayer;
	bool practice;
	SimulationListener* listener;
	std::vector<Shooting> recentShootings;
	std::vector<Casualty> recentCasualties;

	BattleSimulator(BattleModel* battleModel);

	BattleModel* GetBattleModel() const { return _battleModel; }

	void AdvanceTime(float secondsSinceLastTime);

private:
	void SimulateOneTimeStep();

	void RebuildQuadTree();

	void ComputeNextState();
	void AssignNextState();

	void ResolveMeleeCombat();
	void ResolveMissileCombat();

	void TriggerShooting(Unit* unit);
	void ResolveProjectileCasualties();

	void RemoveCasualties();
	void RemoveDeadUnits();

	UnitState NextUnitState(Unit* unit);
	UnitMode NextUnitMode(Unit* unit);
	//glm::vec2 CalculateUnitCenter(Unit* unit);
	float NextUnitDirection(Unit* unit);

	FighterState NextFighterState(Fighter* fighter);
	glm::vec2 NextFighterPosition(Fighter* fighter);
	glm::vec2 NextFighterVelocity(Fighter* fighter);

	Fighter* FindFighterStrikingTarget(Fighter* fighter);
	glm::vec2 CalculateFighterMissileTarget(Fighter* fighter);

	bool IsWithinLineOfFire(Unit* unit, glm::vec2 position);
	Unit* ClosestEnemyWithinLineOfFire(Unit* unit);
};


#endif
