#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "j1FileSystem.h"
#include "j1PerfTimer.h"
#include "p2Point.h"
#include <string>
#include <list>
#include <map>

class Sprite;
class Animation;
class Collider;

enum ACTION_STATE
{
	IDLE = 0,
	WALKING,
	ATTACKING,
	DODGING,
	DAMAGED
};
//TODO: change nums per strings in xml

enum DIRECTION
{
	D_UP = 0,
	D_DOWN,
	D_RIGHT,
	D_LEFT
};

enum ENTITY_TYPE
{
	LINK = 0,
	ENEMY,
	NPC
};


class Entity
{
public:

	Entity();
	Entity(ENTITY_TYPE type);
	~Entity();

	virtual bool Spawn(std::string file, iPoint pos) { return true; };
	virtual bool Update(float dt) { return true; };
	virtual void OnDeath() {};

	virtual bool Draw();

	virtual bool LoadAttributes(pugi::xml_node attributes);
	virtual bool LoadAnimations(std::string file);


	virtual bool Move(int x, int y);
	virtual void UpdateCollider();

	virtual bool GoTo(iPoint dest, int speed, float dt);

public:

	// Position in Entity Manager
	std::list<Entity*>::iterator id;

	// Graphic Resources
	Sprite* sprite;
	Animation* currentAnim;
	std::map<std::pair<ACTION_STATE, DIRECTION>, Animation>	anim;

	// States
	ACTION_STATE actionState;
	DIRECTION currentDir;

	// World Position (to draw)
	iPoint lastPos;
	iPoint currentPos;

	// Collider
	Collider* col;
	iPoint colPivot;
	iPoint appliedForce;

	// Attributes
	ENTITY_TYPE type;
	int life;
	int speed;
	bool damaged;
	j1PerfTimer damagedTimer;

	//Path
	vector<iPoint> path;
	iPoint currentDest;

	//Marked to death
	bool toDelete;

	//Patroling points (in tiles)
	bool patroling;
	vector<iPoint> patrolPoints;
	int currentPatrolPoint;
};

#endif // !_ENTITY_H_