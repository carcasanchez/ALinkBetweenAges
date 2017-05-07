#ifndef _DARK_ZELDA_H_
#define _DARK_ZELDA_H_

#include "Enemy.h"

class j1PerfTimer;
class Object;

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
	bool SummonBolt(float dt);

	bool TeleportAndAttack(float dt);


	void SetAttack();


	void GetHit(Entity* agressor);
	void OnDeath();

private:

	int phase;

	int phase2Life;
	int phase3Life;


	//phase 1 vars
	bool lateralDirection = true;
	j1PerfTimer walkTimer;
	int walkTimelimit;

	j1PerfTimer chargeBowTimer;

	//phase 2
	bool invulnerable = false;
	int dodgeSpeed;
	int dodgeLimit;
	j1PerfTimer dodgeTimer;
	j1PerfTimer holdPosTimer;
	j1PerfTimer holdStabTimer;
	
	int slashSpeed;
	int stabSpeed;

	int newStabSpeed;
	int newSlashSpeed;

	int holdPosLimit;
	int holdStabLimit;

	iPoint lastPlayerPos;

	int teleportRange;
	int stabRange;

	bool preparingStab = false;
	bool holdStab = false;
	bool rage = false;
	int rageLife;
	j1PerfTimer rageTimer;
	int rageLimit;

	//phase 3
	int attackRatio_2;
	int chargeTime_2;


	
	

	Object* bolt = nullptr;

	j1PerfTimer boltTimer;
	int spawnBolt;
	int boltLifeTime;

	j1PerfTimer spinTimer;
	int spinLimit;

	Collider* spinCollider = nullptr;


};

#endif