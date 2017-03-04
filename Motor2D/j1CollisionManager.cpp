#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1CollisionManager.h"


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

	//LOAD DATA FROM CONFIG XML

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
			App->render->DrawQuad((*it)->rect, 0, 255, 255, 1000);
		}
	}
	return false;
}