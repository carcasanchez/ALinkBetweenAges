#ifndef __MANAGER_H__
#define __MANAGER_H__


#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <map>
#include <string>
#include <vector>
#include "p2Point.h"



class Entity;
class Player;
class Enemy;
class Npc;
class Object;
enum ENTITY_TYPE;
enum ENEMY_TYPE;
enum NPC_TYPE;
enum LINK_AGE;
enum OBJECT_TYPE;

class j1EntityManager
{
public:

	j1EntityManager();
	bool Awake(pugi::xml_node&);
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	Player* CreatePlayer(int, int, LINK_AGE);
	Enemy* CreateEnemy(int, ENEMY_TYPE, int, int, std::vector<iPoint>, int id = -1);
	Npc*	CreateNPC(int, NPC_TYPE, int, int, int id = -1);
	Object* CreateObject(int, int, int, OBJECT_TYPE, int id = -1);

	Entity* GetEntityFromId(int);

	bool CleanEntities();
	void SetSectorRef(int*);

public:

	Player* player;
	std::map<int, std::list<Entity*>> entities;
	std::map<ENTITY_TYPE, std::string> dir;

private:

	int* sector;

};

#endif