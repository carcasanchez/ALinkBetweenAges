#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Entity.h"
#include "j1PerfTimer.h"

enum ENEMY_TYPE
{
	NONE = 0,
	GREEN_SOLDIER,
	BLUE_SOLDIER,
	RED_SOLDIER,
	DARK_ZELDA,
	OCTOROK
};

enum ENEMY_STATE
{
	PATROLING = 0, 
	CHASING,
	KEEP_DISTANCE,
	STEP_BACK,
	CHARGING
};

class Enemy : public Entity
{
public:
	Enemy() :Entity() {};


	virtual bool Spawn(std::string file, iPoint pos) { return true; };
	virtual bool Update(float dt) { return true; };
	virtual void OnDeath();





	//State Machine
	virtual bool Patroling(float dt);
	virtual bool Chasing(float dt);
	virtual bool KeepDistance(float dt);
	virtual bool StepBack(float dt);
	virtual bool Charging(float dt);


public:
	ENEMY_STATE enemyState;
	
	bool flankingDir;

	//Combat ranges
	int hostileRange;
	int fightRange;
	int outFightRange;

	//combat ratios
	int attackRatio;
	int chargeTime;

	//combat speeds
	int chaseSpeed;
	int flankingSpeed;
	int attackSpeed;
	
	//Patroling points (in tiles)
	bool patroling;
	vector<iPoint> patrolPoints;
	int currentPatrolPoint = 0;

protected:
	iPoint currentDest = { 0, 0 };

	//Combat behaviour variables
	j1PerfTimer attackTimer;


};

#endif
