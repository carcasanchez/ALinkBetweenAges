#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "j1FileSystem.h"
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
	DODGING
};

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


	virtual void Move(int x, int y);
	virtual void UpdateCollider();

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

	// Attributes
	ENTITY_TYPE type;
	int life;
	int speed;

	//Path
	vector<iPoint> path;
};

#endif // !_ENTITY_H_