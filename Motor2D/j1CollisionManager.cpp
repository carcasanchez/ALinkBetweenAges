#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1CollisionManager.h"
#include "j1Map.h"
#include "j1Pathfinding.h"



j1CollisionManager::j1CollisionManager() : j1Module()
{
}

j1CollisionManager::~j1CollisionManager()
{
}

bool j1CollisionManager::Awake(pugi::xml_node & config)
{
	LOG("Creating Collision Manager");
	bool ret = true;

	
	//Player collisions
	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_NPC] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PICKABLE_ITEM] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_BUSH] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_OCTOSTONE] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_CHEST] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_MAGIC_SLASH] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_BUYABLE_ITEM] = true; //	Shop
	matrix[COLLIDER_PLAYER][COLLIDER_EVENT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_BOLT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_BOMB_EXPLOSION] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ZELDA_ARROW] = true;

	matrix[COLLIDER_BUYABLE_ITEM][COLLIDER_LINK_SWORD] = true; //	Shop
	matrix[COLLIDER_BUSH][COLLIDER_LINK_SWORD] = true;
	matrix[COLLIDER_CHEST][COLLIDER_LINK_SWORD] = true;
	matrix[COLLIDER_NPC][COLLIDER_LINK_SWORD] = true;
	matrix[COLLIDER_OCTOSTONE][COLLIDER_LINK_SWORD] = true;

	//Enemy collisions
	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_LINK_SWORD] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_LINK_ARROW] = true;
	matrix[COLLIDER_EYE][COLLIDER_LINK_ARROW] = true;
	matrix[COLLIDER_BOMB_EXPLOSION][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_BOMB][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_BOMB][COLLIDER_ENEMY] = true;

	return false;
}


bool j1CollisionManager::PreUpdate()
{
	//Destroy all to-delete colliders 	
	
	for (std::list <Collider*>::iterator it = colliders.begin(); it != colliders.end(); it++)
	{	
		if ((*it)->to_delete)
		{
			colliders.erase(it);
		}
	}


	return false;
}

bool j1CollisionManager::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;


	Collider* c1;
	Collider* c2;

	for (std::list <Collider*>::iterator it = colliders.begin(); it != colliders.end();)
	{
		c1 = (*it);
		it++;

		//Check collision with other colliders  //Avoid checking collisions already checked
		for (std::list <Collider*>::iterator it2 = it; it2 != colliders.end(); it2++)
		{
			c2 = (*it2);
			
			if (c1->CheckCollision(c2->rect) && c1->active && c2->active)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->On_Collision_Callback(c1, c2, dt);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->On_Collision_Callback(c2, c1, dt);
			}
		}
	}

	return true;
}


bool j1CollisionManager::CleanUp()
{
	LOG("Freeing all colliders");
	for (std::list <Collider*>::iterator it = colliders.begin(); it != colliders.end(); it++)
	{
		colliders.erase(it);
	}

	return true;
}


//Utility
Collider* j1CollisionManager::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module * callback)
{
	Collider* ret = new Collider(type, rect, callback);
	colliders.push_back(ret);

	return ret;
}


bool j1CollisionManager::DrawDebug()
{
	if (debug)
	{
		for (std::list <Collider*>::iterator it = colliders.begin(); it != colliders.end(); it++)
		{
			App->render->DrawQuad((*it)->rect, 255, 255, 0, 150);
		}
	}
	return false;
}

//------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}

COLLISION_ZONE Collider::CheckPlayerMapCollision()
{
	COLLISION_ZONE ret = CZ_NONE;

	iPoint up_left = App->map->WorldToMap(rect.x, rect.y);
	iPoint up_right = App->map->WorldToMap(rect.x + rect.w, rect.y); 
	iPoint down_left = App->map->WorldToMap(rect.x, rect.y + rect.h);
	iPoint down_right = App->map->WorldToMap(rect.x + rect.w, rect.y + rect.h); 

	if (App->pathfinding->IsPlayerWalkable(up_left) == false)
	{
		ret = CZ_UP_LEFT;
		if (App->pathfinding->IsPlayerWalkable(up_right) == false)
			ret = CZ_UP;
		else if (App->pathfinding->IsPlayerWalkable(down_left) == false)
			ret = CZ_LEFT;
	}
	else if (App->pathfinding->IsPlayerWalkable(down_right) == false)
	{
		ret = CZ_DOWN_RIGHT;
		if (App->pathfinding->IsPlayerWalkable(up_right) == false)
			ret = CZ_RIGHT;
		else if (App->pathfinding->IsPlayerWalkable(down_left) == false)
			ret = CZ_DOWN;
	}
	else if (App->pathfinding->IsPlayerWalkable(up_right) == false)
		ret = CZ_UP_RIGHT;
	else if (App->pathfinding->IsPlayerWalkable(down_left) == false)
		ret = CZ_DOWN_LEFT;


	return ret;
}

COLLISION_ZONE Collider::CheckEnemyMapCollision()
{
	COLLISION_ZONE ret = CZ_NONE;

	iPoint up_left = App->map->WorldToMap(rect.x, rect.y);
	iPoint up_right = App->map->WorldToMap(rect.x + rect.w, rect.y);
	iPoint down_left = App->map->WorldToMap(rect.x, rect.y + rect.h);
	iPoint down_right = App->map->WorldToMap(rect.x + rect.w, rect.y + rect.h);

	if (App->pathfinding->IsEnemyWalkable(up_left) == false)
	{
		ret = CZ_UP_LEFT;
		if (App->pathfinding->IsEnemyWalkable(up_right) == false)
			ret = CZ_UP;
		else if (App->pathfinding->IsEnemyWalkable(down_left) == false)
			ret = CZ_LEFT;
	}
	else if (App->pathfinding->IsEnemyWalkable(down_right) == false)
	{
		ret = CZ_DOWN_RIGHT;
		if (App->pathfinding->IsEnemyWalkable(up_right) == false)
			ret = CZ_RIGHT;
		else if (App->pathfinding->IsEnemyWalkable(down_left) == false)
			ret = CZ_DOWN;
	}
	else if (App->pathfinding->IsEnemyWalkable(up_right) == false)
		ret = CZ_UP_RIGHT;
	else if (App->pathfinding->IsEnemyWalkable(down_left) == false)
		ret = CZ_DOWN_LEFT;


	return ret;
}

COLLISION_ZONE Collider::CheckPlayerMapJump()
{
	COLLISION_ZONE ret = CZ_NONE;

	iPoint up_left = App->map->WorldToMap(rect.x, rect.y);
	iPoint up_right = App->map->WorldToMap(rect.x + rect.w, rect.y);
	iPoint down_left = App->map->WorldToMap(rect.x, rect.y + rect.h);
	iPoint down_right = App->map->WorldToMap(rect.x + rect.w, rect.y + rect.h);

	if (App->pathfinding->IsPlayerJumpable(up_left))
	{
		ret = CZ_UP_LEFT;
		if (App->pathfinding->IsPlayerJumpable(up_right))
			ret = CZ_UP;
		else if (App->pathfinding->IsPlayerJumpable(down_left))
			ret = CZ_LEFT;
	}
	else if (App->pathfinding->IsPlayerJumpable(down_right))
	{
		ret = CZ_DOWN_RIGHT;
		if (App->pathfinding->IsPlayerJumpable(up_right))
			ret = CZ_RIGHT;
		else if (App->pathfinding->IsPlayerJumpable(down_left))
			ret = CZ_DOWN;
	}

	else if (App->pathfinding->IsPlayerJumpable(up_right))
		ret = CZ_UP_RIGHT;
	else if (App->pathfinding->IsPlayerJumpable(down_left))
		ret = CZ_DOWN_LEFT;
	
	return ret;
}


COLLISION_ZONE Collider::CheckArrowCollision()
{
	COLLISION_ZONE ret = CZ_NONE;

	iPoint up_left = App->map->WorldToMap(rect.x, rect.y);
	iPoint up_right = App->map->WorldToMap(rect.x + rect.w, rect.y);
	iPoint down_left = App->map->WorldToMap(rect.x, rect.y + rect.h);
	iPoint down_right = App->map->WorldToMap(rect.x + rect.w, rect.y + rect.h);

	if (App->pathfinding->CheckArrowCollision(up_left) == false ||
		App->pathfinding->CheckArrowCollision(up_right) == false ||
		App->pathfinding->CheckArrowCollision(down_left) == false ||
		App->pathfinding->CheckArrowCollision(down_right) == false)
	{
		ret = CZ_UP_LEFT;

	}
	


	return ret;
}