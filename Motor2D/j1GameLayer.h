#ifndef _GAME_LAYER_H_
#define _GAME_LAYER_H_

#include "j1Module.h"
#include <list>
#include "j1Timer.h"

using namespace std;

class Entity;
class j1EntityManager;
class Hud;
class Collider;
class Object;


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

	void PickObject(Object* object);
	void BuyObject(Object* object);

	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);

	bool On_Collision_Callback(Collider*, Collider*, float dt);

public:

	j1EntityManager*	em = NULL;
	Hud*				hud = NULL;

	bool        pause = false;
	bool		quit_game = false;

	int playerX = 9;
	int playerY = 5;

	j1Timer buy_timer; 

};



#endif // !_GAME_LAYER_H_