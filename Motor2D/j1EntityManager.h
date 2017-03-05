#ifndef __MANAGER_H__
#define __MANAGER_H__

#include "Entity.h"
#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <string>

struct SDL_Texture;
class Entity;


using namespace std;

class j1EntityManager
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

	//Entity* Create();

	//list<Entity*> Entities;

	//string		Entity_texture_name;
	//SDL_Texture*	Entity_textures;
};

#endif