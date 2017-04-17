#ifndef _RED_SOLDIER_H_
#define _RED_SOLDIER_H_

#include "Enemy.h"

class RedSoldier : public Enemy
{
public:
	RedSoldier() :Enemy() {};
	~RedSoldier() {};

	bool Spawn(std::string file, iPoint pos);
	bool Update(float dt);
	void OnDeath();
};

#endif