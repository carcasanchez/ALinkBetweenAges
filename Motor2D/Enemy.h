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
	
	bool Enemy::Spawn(std::string file, iPoint pos)
	{
		//TODO: ENEMY DATA LOAD
		return true;
	}

};

#endif
