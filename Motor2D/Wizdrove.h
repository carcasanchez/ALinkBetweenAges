#ifndef _WIZDROVE_H_
#define _WIZDROVE_H_

#include "Enemy.h"

class Wizdrove : public Enemy
{
public:
	Wizdrove() :Enemy() {};
	~Wizdrove() {};

	bool Spawn(std::string file, iPoint pos);
	bool Update(float dt);

	bool Patroling(float dt);
	bool ThrowingAttack(float dt);
	bool PreparingAttack(float dt);

	//void OnDeath();



};

#endif
