#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Entity.h"
#include "j1PerfTimer.h"

enum ENEMY_TYPE
{
	NONE = 0,
	GREEN_SOLDIER,
	BLUE_SOLDIER,
	RED_SOLDIER
};

class Enemy : public Entity
{
public:
	Enemy() :Entity() {};
	
	virtual bool Spawn(std::string file, iPoint pos) { return true; };
	virtual bool Update(float dt);
	virtual bool SearchForPlayer(int speed, float dt);
	virtual void OnDeath();

protected:
	iPoint currentDest = { 0, 0 };

private:
	iPoint movement;
};

#endif
