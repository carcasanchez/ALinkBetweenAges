#ifndef _GREEN_SOLDIER_H_
#define _GREEN_SOLDIER_H_

#include "Enemy.h"

class GreenSoldier : public Enemy
{
public:
	GreenSoldier():Enemy() {};
	~GreenSoldier() {};

	bool Spawn(std::string file, iPoint pos);
	bool Update(float dt);

};

#endif