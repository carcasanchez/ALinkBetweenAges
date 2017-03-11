#ifndef _GAME_LAYER_H_
#define _GAME_LAYER_H_

#include "j1Module.h"
#include <list>

using namespace std;

class Entity;
class j1EntityManager;
class Hud;
class Collider;

class j1GameLayer : public j1Module
{
public:
	j1GameLayer();
	~j1GameLayer();

	bool Awake(pugi::xml_node& conf);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();


	bool On_Collision_Callback(Collider*, Collider*, float dt);

public:

	j1EntityManager*	em = NULL;
	Hud*				hud = NULL;

	std::list<Entity*>::iterator playerId;

	bool        pause = false;
};



#endif // !_GAME_LAYER_H_