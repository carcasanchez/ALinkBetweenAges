#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "Entity.h"
#include "Module_entites_manager.h"
#include "p2List.h"
#include "p2Point.h"

class Player : public entity
{
public:

	Player(p2Point<int> pos);
	bool Start();
	bool UTicks();
	bool PreU();
	bool U(float dt);
	bool PostU();


private:

	p2List<iPoint>		path;
	bool				on_path = false;
	



	
};

#endif