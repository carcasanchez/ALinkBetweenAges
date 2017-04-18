#ifndef _DARK_ZELDA_H_
#define _DARK_ZELDA_H_

#include "Enemy.h"

class DarkZelda : public Enemy
{
public:
	DarkZelda() :Enemy() {};
	~DarkZelda() {};

	bool Spawn(std::string file, iPoint pos);
	bool Update(float dt);
	// OnDeath();

public:
	int phase = 1;
};

#endif