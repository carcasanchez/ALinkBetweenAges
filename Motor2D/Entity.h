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
	DAMAGED,
	SHOOTING_BOW,
	SPINNING,
	OPEN,
	CLOSE,
	JUMPING,
	APPEARING,
	DISAPPEARING,
	STABBING,
	SPECIAL_ATTACK,
	MAGIC_ATTACK,
	SPECIAL_STAB,
	PREPARING_SWORD,
	HOLDING_NORMAL_SWORD,
	HOLDING_FIRE_SWORD,
	SUMMONING

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
	NPC,
	OBJECT
};


class Entity
{
public:

	Entity();
	Entity(ENTITY_TYPE type);
	virtual ~Entity();

	virtual bool Spawn(std::string file, iPoint pos) { return true; };
	virtual bool Update(float dt) { return true; };
	virtual void OnDeath() {};

	virtual bool Draw();

	virtual bool LoadAttributes(pugi::xml_node attributes);
	virtual bool LoadAnimations(std::string file);


	virtual bool Move(int x, int y);
	virtual bool MoveTo(int, int);
	virtual void UpdateCollider();

	virtual bool GoTo(iPoint dest, int speed, float dt);
	virtual bool LookToPlayer();

	virtual void GetHit(Entity* agressor) 
	{
		life -= agressor->damage;
		return; 
	};

public:
	// Position in Entity Manager
	int id;

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
	iPoint linearMovement;

	// Attributes
	ENTITY_TYPE type;
	int life;
	int speed;
	int damage;
	bool damaged;
	j1PerfTimer damagedTimer;
	j1PerfTimer pushedBackTimer;

	//Path
	vector<iPoint> path;
	iPoint currentDest;

	//Marked to death
	bool toDelete;
	bool keepExisting = false;

private:
	int textureFile = 1;
	int animationFile = 1;

protected:
	int damagedLimit = 100;
	
};

#endif // !_ENTITY_H_