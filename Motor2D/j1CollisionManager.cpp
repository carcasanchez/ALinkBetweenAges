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

	//TODO: LOAD DATA FROM CONFIG XML
	
	//Player collisions
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;

	//Wall collisions
	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;

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

			
			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->On_Collision_Callback(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->On_Collision_Callback(c2, c1);
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
			App->render->DrawQuad((*it)->rect, 255, 0, 0, 100);
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

bool Collider::CheckMapCollision()
{
	bool ret = false;

	iPoint up_left = App->map->WorldToMap(rect.x, rect.y);
	iPoint up_right = App->map->WorldToMap(rect.x + rect.w, rect.y); 
	iPoint down_left = App->map->WorldToMap(rect.x, rect.y + rect.h);
	iPoint down_right = App->map->WorldToMap(rect.x + rect.w, rect.y + rect.h); 

	if (App->pathfinding->IsWalkable(up_left) == false)
		ret = true; 

	if (App->pathfinding->IsWalkable(up_right) == false)
		ret = true;

	if (App->pathfinding->IsWalkable(down_left) == false)
		ret = true;

	if (App->pathfinding->IsWalkable(down_right) == false)
		ret = true;

	return ret;
}
