#ifndef _OCTOROK_H_
#define _OCTOROK_H_

#include "Enemy.h"

class Octorok : public Enemy
{
public:
	Octorok() :Enemy() {};
	~Octorok() {};

	bool Spawn(std::string file, iPoint pos);
	bool Update(float dt);
	//void OnDeath();

};

#endif