#include "j1GameLayer.h"
#include "j1App.h"
#include "j1EntityManager.h"
#include "j1Gui.h"
#include "j1CollisionManager.h"
#include "p2Point.h"
#include "j1Render.h"
#include "Enemy.h"
#include "Player.h"
#include "j1PerfTimer.h"
#include "p2Log.h"
#include "j1SceneManager.h"
#include "Scene.h"
//#include "Hud.h"


// just for temporal wall collider
#include "Entity.h"


j1GameLayer::j1GameLayer() : j1Module()
{
	name = ("game");
	em = new j1EntityManager();
	//playerId = em->entities[App->sceneM->currentScene->currentSector].end();
	//hud = new Hud();
}

j1GameLayer::~j1GameLayer()
{
	RELEASE(hud);
	RELEASE(em);
}

bool j1GameLayer::Awake(pugi::xml_node& conf)
{
	em->Awake(conf);
	//hud->Awake(conf);
	return true;
}

// Called before the first frame
bool j1GameLayer::Start()
{
	active = true;


	em->CreatePlayer(50, 50);


	return true;
}

//preUpdate
bool j1GameLayer::PreUpdate()
{
	em->PreUpdate();
	//hud->PreUpdate();

	return true;
}

//update
bool j1GameLayer::Update(float dt)
{
	bool ret = true;

	em->Update(dt);
	//ret = hud->Update(dt);

	App->render->CameraFollow((*playerId)->currentPos);

	if(App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		em->CreateEnemy(1, GREEN_SOLDIER, 16, 16);
	
	return ret;
}

//postUpdate
bool j1GameLayer::PostUpdate()
{
	em->PostUpdate();
	//hud->PostUpdate();

	return true;
}

// Called before quitting
bool j1GameLayer::CleanUp()
{
	active = false;

	//hud->CleanUp();
	em->CleanUp();

	return true;
}

bool j1GameLayer::On_Collision_Callback(Collider * c1, Collider * c2 , float dt)
{
	if (c2->type == COLLIDER_WALL || c2->type == COLLIDER_NPC)
	{
		if((*playerId) != nullptr)
			(*playerId)->currentPos = (*playerId)->lastPos;
		return true;
	}

	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_ENEMY)
	{
		if (((Player*)(*playerId))->invulnerable == false)
		{

			if ((*playerId) != nullptr)
			{
				(*playerId)->damaged = ((Player*)(*playerId))->invulnerable = true;
				(*playerId)->damagedTimer.Start();
				LOG("DAMAGED TIMER STARTED");
				(*playerId)->life--;
				(*playerId)->sprite->tint = { 100, 0, 0, 255 };

				if (c1->rect.x < c2->rect.x)
					(*playerId)->appliedForce.x = -1;
				else (*playerId)->appliedForce.x = 1;

				if (c1->rect.y < c2->rect.y)
					(*playerId)->appliedForce.y = -1;
				else (*playerId)->appliedForce.y = 1;			
			}
		}

		return true;
	}

	if (c1->type == COLLIDER_ENEMY && c2->type == COLLIDER_LINK_SWORD)
	{
		c1->parent->life--;
	}
	
	if (c1->type == COLLIDER_ENEMY && c2->type == COLLIDER_ENEMY)
	{
		if (c1->parent->currentPos.DistanceTo(c2->parent->currentPos) > 6)
			return true;

		if (c1->rect.x < c2->rect.x)
			c2->parent->currentPos.x += c2->parent->speed*dt * 5;
		else c2->parent->currentPos.x += -c2->parent->speed*dt * 5;

		if (c1->rect.y < c2->rect.y)
			c2->parent->currentPos.y += c2->parent->speed*dt * 5;
		else c2->parent->currentPos.y -= c2->parent->speed*dt * 5;

		c2->parent->UpdateCollider();
	}
		
	return true;
}
