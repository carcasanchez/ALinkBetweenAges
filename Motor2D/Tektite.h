#ifndef _TEKTITE_H_
#define _TEKTITE_H_

#include "Enemy.h"

class Tektite : public Enemy
{
public:
	Tektite() :Enemy() {};
	~Tektite() {};

	bool Spawn(std::string file, iPoint pos);
	bool Update(float dt);
	//void OnDeath();

	bool Chasing(float dt);
	void OnDeath();
};

#endif#pragma once
