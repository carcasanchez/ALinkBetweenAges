#ifndef _GAME_LAYER_H_
#define _GAME_LAYER_H_

#include "j1Module.h"
#include <list>

using namespace std;

class Player;
class j1EntityManager;
class Hud;

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

public:

	Player*				player = NULL;
	j1EntityManager*	em = NULL;
	Hud*				hud = NULL;

	bool        pause = false;
};



#endif // !_GAME_LAYER_H_