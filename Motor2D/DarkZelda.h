#ifndef _DARK_ZELDA_H_
#define _DARK_ZELDA_H_

#include "Enemy.h"

class j1PerfTimer;

class DarkZelda : public Enemy
{
public:
	DarkZelda() :Enemy() {};
	~DarkZelda() {};

	bool Spawn(std::string file, iPoint pos);
	bool Update(float dt);

	bool LateralWalk(float dt);
	bool ChargeBow(float dt);
	bool KeepDistance(float dt);
	bool StepBack(float dt);
	bool Chasing(float dt);
	bool Dodging(float dt);
	bool Charging(float dt);
	bool Attack(float dt);
	bool Stab(float dt);
	bool Spin(float dt);

	bool TeleportAndAttack(float dt);


	void SetAttack();


	bool GetHit();
	void OnDeath();

private:

	int phase = 1;

	//phase 1 vars
	bool lateralDirection = true;
	j1PerfTimer walkTimer;
	int walkTimelimit;

	j1PerfTimer chargeBowTimer;

	//phase 2
	int dodgeSpeed;
	int dodgeLimit;
	j1PerfTimer dodgeTimer;
	j1PerfTimer holdPosTimer;
	j1PerfTimer holdStabTimer;
	
	int holdPosLimit;
	int holdStabLimit;

	iPoint lastPlayerPos;

	int teleportRange;

	bool holdStab = false;

	//phase 3
	int attackRatio_2;

	


};

#endif