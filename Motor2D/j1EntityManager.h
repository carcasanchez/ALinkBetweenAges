#ifndef __MANAGER_H__
#define __MANAGER_H__


#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <map>
#include <string>

class Entity;
enum ENTITY_TYPE;

class j1EntityManager
{
public:

	j1EntityManager();
	bool Awake(pugi::xml_node& config);
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

public:

	Entity* Create(ENTITY_TYPE type, int x, int y);

	std::list<Entity*> entities;

	std::map<ENTITY_TYPE, std::string> dir;
};

#endif