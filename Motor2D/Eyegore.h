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

	bool Chasing(float dt);
	bool StepBack(float dt);
	//void OnDeath();

private:

	int rageSpeed;
	int rageLife;

};

#endif