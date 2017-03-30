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
#include "j1Map.h"
#include "HUD.h"


// just for temporal wall collider
#include "Entity.h"


j1GameLayer::j1GameLayer() : j1Module()
{
	name = ("game");
	em = new j1EntityManager();
	//playerId = em->entities[App->sceneM->currentScene->currentSector].end();
	hud = new Hud();
}

j1GameLayer::~j1GameLayer()
{
	RELEASE(hud);
	RELEASE(em);
}

bool j1GameLayer::Awake(pugi::xml_node& conf)
{
	em->Awake(conf);
	hud->Awake(conf);
	return true;
}

// Called before the first frame
bool j1GameLayer::Start()
{
	active = true;

	hud->Start();
	em->CreatePlayer(107, 232);


	return true;
}

//preUpdate
bool j1GameLayer::PreUpdate()
{
	if (!pause)
		em->PreUpdate();
	//hud->PreUpdate();

	return true;
}

//update
bool j1GameLayer::Update(float dt)
{
	bool ret = true;

	if(!pause)
		em->Update(dt);
	
	ret = hud->Update(dt);

	App->render->CameraFollow((*playerId)->currentPos);

	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		iPoint mousePos = App->map->WorldToMap((*playerId)->currentPos.x, (*playerId)->currentPos.y);
		em->CreateEnemy(1, GREEN_SOLDIER, mousePos.x-5, mousePos.y-5, vector<iPoint>());
	}
		
	
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
		iPoint Movement;
		if (abs(c1->rect.x - c2->rect.x) < abs(c1->rect.y - c2->rect.y))
		{
			if (c1->parent->currentPos.y > c2->parent->currentPos.y)
				Movement = {0, 1};
			else Movement = { 0, -1 };
		}
		else
		{
			if (c1->parent->currentPos.x > c2->parent->currentPos.x)
				Movement = { 1, 0 };
			else Movement = { -1, 0 };
		}

		c1->parent->Move(SDL_ceil(c1->parent->speed*dt)*Movement.x, SDL_ceil(c1->parent->speed*dt)*Movement.y);
				
		return true;
	}

	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_ENEMY)
	{
		if (((Player*)(*playerId))->invulnerable == false && ((Player*)(*playerId))->actionState != DODGING)
		{

 			if ((*playerId) != nullptr)
			{
				(*playerId)->damaged = ((Player*)(*playerId))->invulnerable = true;
				(*playerId)->damagedTimer.Start();
				LOG("DAMAGED TIMER STARTED");
				(*playerId)->life -= c2->parent->damage;
				(*playerId)->sprite->tint = { 100, 0, 0, 255 };

				if (c1->rect.x < c2->rect.x)
					(*playerId)->linearMovement.x = -1;
				else (*playerId)->linearMovement.x = 1;

				if (c1->rect.y < c2->rect.y)
					(*playerId)->linearMovement.y = -1;
				else (*playerId)->linearMovement.y = 1;
			}
		}

		return true;
	}

	if (c1->type == COLLIDER_ENEMY)
	{
		if (c2->type == COLLIDER_LINK_SWORD)
		{
			if (((Enemy*)(c1->parent))->enemyState != STEP_BACK)
			{
				c1->parent->life -= (*playerId)->damage;
				c1->parent->sprite->tint = { 255, 150, 150, 255 };
				((Enemy*)(c1->parent))->enemyState = STEP_BACK;
				c1->parent->damagedTimer.Start();
			}
			return true;
		}

		if (c2->type == COLLIDER_ENEMY)
		{
		/*	switch (((Enemy*)(c2->parent))->enemyState)
			{
			case KEEP_DISTANCE:
				break;
			case CHASING:
				iPoint Movement;
				if (c1->parent->currentPos.DistanceTo(c2->parent->currentPos) > 10)
					break;

				if (c1->rect.x < c2->rect.x)
					Movement.x = 1;
				else Movement.x = -1;;

				if (c1->rect.y < c2->rect.y)
					Movement.y = 1;
				else Movement.y = -1;

				c2->parent->Move(SDL_ceil(c2->parent->speed*dt * 5)*Movement.x, SDL_ceil(c2->parent->speed*dt * 5)*Movement.y);
				break;
		}*/
			return true;
	}
	}

	if (c1->type == COLLIDER_NPC)
	{
		if (c2->type == COLLIDER_LINK_SWORD)
		{
			((Player*)(*playerId))->toTalk = (Npc*)c1->parent;
		}
	}
		
	return true;
}
