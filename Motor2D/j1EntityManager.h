#ifndef __MANAGER_H__
#define __MANAGER_H__


#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <map>
#include <string>

class Entity;
class Player;
class Enemy;
enum ENTITY_TYPE;
enum ENEMY_TYPE;

class j1EntityManager
{
public:

	j1EntityManager();
	bool Awake(pugi::xml_node&);
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

public:

	Player* CreatePlayer(int, int);
	Enemy* CreateEnemy(int, ENEMY_TYPE, int, int);

	std::map<int, std::list<Entity*>> entities;

	std::map<ENTITY_TYPE, std::string> dir;
};

#endif