#ifndef _EYEGORE_H_
#define _EYEGORE_H_

#include "Enemy.h"

class Eyegore : public Enemy
{
public:
	Eyegore() :Enemy() {};
	~Eyegore() {};

	bool Spawn(std::string file, iPoint pos);
	bool Update(float dt);
	//void OnDeath();
};

#endif