#ifndef _COLLISION_MANAGER_H_
#define _COLLISION_MANAGER_H_


#include "SDL/include/SDL.h"
#include "j1Module.h"
#include "p2Point.h"


enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_WALL,
	COLLIDER_PLAYER
};
class Collider
{
public:
	SDL_Rect rect;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;
	bool to_delete = false;
	

	Collider(COLLIDER_TYPE type, SDL_Rect rect, j1Module* callback = nullptr) :type(type), callback(callback), rect(rect) {};
	
	//bool CheckCollision(const Collider* c) const;

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

	bool debug = false;
	std::list <Collider*> colliders;
};



#endif // !_COLLISION_MANAGER_H_