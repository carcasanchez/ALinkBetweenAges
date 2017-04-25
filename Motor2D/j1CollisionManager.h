#ifndef _COLLISION_MANAGER_H_
#define _COLLISION_MANAGER_H_


#include "SDL/include/SDL.h"
#include "j1Module.h"
#include "Entity.h"
#include "p2Point.h"

#define MAX_COLLIDERS 1000


enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_WALL,
	COLLIDER_PLAYER,
	COLLIDER_NPC,
	COLLIDER_ENEMY,
	COLLIDER_LINK_SWORD,
	COLLIDER_EXIT,
	COLLIDER_LINK_ARROW,
	COLLIDER_PICKABLE_ITEM,
	COLLIDER_BUSH,
	COLLIDER_OCTOSTONE,
	COLLIDER_CHEST,
	COLLIDER_MAGIC_SLASH,
	COLLIDER_BUYABLE_ITEM,
	COLLIDER_EYE,
	COLLIDER_BOMB
};


enum COLLISION_ZONE
{
	CZ_NONE = -1,
	CZ_UP,
	CZ_DOWN,
	CZ_LEFT,
	CZ_RIGHT,
	CZ_UP_LEFT,
	CZ_UP_RIGHT,
	CZ_DOWN_LEFT,
	CZ_DOWN_RIGHT
};
class Collider
{
public:
	SDL_Rect rect;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;
	bool to_delete = false;
	bool active = true;

	Entity* parent = NULL;
	

	Collider(COLLIDER_TYPE type, SDL_Rect rect, j1Module* callback = nullptr) :type(type), callback(callback), rect(rect) {};
	
	bool CheckCollision(const SDL_Rect& r) const;
	COLLISION_ZONE CheckPlayerMapCollision();
	COLLISION_ZONE CheckEnemyMapCollision();
	COLLISION_ZONE CheckPlayerMapJump();
};


class j1CollisionManager : public j1Module
{
public:

	j1CollisionManager();
	~j1CollisionManager();
	
	bool Awake(pugi::xml_node& config);

	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	bool DrawDebug();

private:

	bool matrix[MAX_COLLIDERS][MAX_COLLIDERS];
	bool debug = false;
	std::list <Collider*> colliders;
};



#endif // !_COLLISION_MANAGER_H_