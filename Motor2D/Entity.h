#ifndef _ENTITY_H_
#define _ENTITY_H_

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
	NPC1
};


class Entity
{
public:

	Entity();
	Entity(ENTITY_TYPE type);
	~Entity();

	virtual bool Spawn(std::string file, iPoint pos) { return true; };
	virtual bool Update(float dt) { return true; };
	virtual bool Draw();
	virtual void OnDeath() {};

	virtual bool LoadAnimations() { return true; };

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
};

#endif // !_ENTITY_H_