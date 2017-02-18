#ifndef __MANAGER_H__
#define __MANAGER_H__

#include "j1Module.h"
#include "p2Point.h"

//#include "Entity.h"


struct SDL_Texture;
class entity;

class j1EntityManager : public j1Module
{
public:

	j1EntityManager();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PreUpdate();
	bool UpdateTicks();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void Entity_selected();
	void Entity_disselected();

public:

	entity* create(p2Point<int> position);

	list<entity*> Entities;

	string		Entity_texture_name;
	SDL_Texture*	Entity_textures;


};

#endif