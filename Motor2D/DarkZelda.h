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
	void OnDeath();

private:

	int phase = 1;

	//phase 1 vars
	bool lateralDirection = true;
	j1PerfTimer walkTimer;
	int walkTimelimit;

	j1PerfTimer chargeBowTimer;
	int bowTenseLimit;

};

#endif