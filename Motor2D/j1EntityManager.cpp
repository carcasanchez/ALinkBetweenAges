#include "j1EntityManager.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1GameLayer.h"
#include "Entity.h"
#include "Player.h"
#include "Enemy.h"
#include "NPC.h"
#include "j1CollisionManager.h"
#include "j1SceneManager.h"
#include "Scene.h"
#include "j1Map.h"

//=====Enemy Includes
#include "GreenSoldier.h"


j1EntityManager::j1EntityManager()
{
	
}

bool j1EntityManager::Awake(pugi::xml_node & config)
{
	bool ret = true;


	pugi::xml_node entityAttributes = config.child("entityAttributes");
	std::string folder = entityAttributes.attribute("folder").as_string("/");
	pugi::xml_node entity = entityAttributes.first_child();

	for (int tmp = 0; !entity.empty(); tmp++)
	{
		dir.insert(std::pair<ENTITY_TYPE, std::string >(ENTITY_TYPE(tmp), (folder + entity.attribute("file").as_string(".xml"))));
		entity = entity.next_sibling();
	}

	return ret;
}

bool j1EntityManager::PreUpdate()
{
	bool ret = true;

	// check for dead entities
	std::list<Entity*>::iterator item = entities[App->sceneM->currentScene->currentSector].begin();
	while (item != entities[App->sceneM->currentScene->currentSector].end())
	{
		if ((*item)->life < 0)
		{
			(*item)->OnDeath();
			if((*item)->toDelete)
				item = entities[App->sceneM->currentScene->currentSector].erase(item); //calls destroyer
		}
		else
		{
			++item;
		}

		// reasign ids
		if(item != entities[App->sceneM->currentScene->currentSector].end())
			(*item)->id = item;
	}

	return ret;
}

bool j1EntityManager::Update(float dt)
{
	bool ret = true;

	for (std::list<Entity*>::iterator item = entities[App->sceneM->currentScene->currentSector].begin(); item != entities[App->sceneM->currentScene->currentSector].end(); item++)
	{		
		if (App->render->InsideCameraZone((*item)->col->rect))
		{
			(*item)->Update(dt);
			(*item)->UpdateCollider();
		}
		
	}

	return ret;
}

bool j1EntityManager::PostUpdate()
{
	bool ret = true;

	for (std::list<Entity*>::iterator item = entities[App->sceneM->currentScene->currentSector].begin(); item != entities[App->sceneM->currentScene->currentSector].end(); item++)
	{
		(*item)->Draw();
	}

	return ret;
}

bool j1EntityManager::CleanUp()
{
	bool ret = true;

	for (std::list<Entity*>::iterator item = entities[App->sceneM->currentScene->currentSector].begin(); item != entities[App->sceneM->currentScene->currentSector].end(); item++)
	{
		entities[App->sceneM->currentScene->currentSector].erase(item);
	}

	return ret;
}

Player* j1EntityManager::CreatePlayer(int x, int y)
{
	Player* ret = new Player();


	iPoint worldPos = App->map->GetTileCenter(iPoint(x, y));

	ret->Spawn(dir[LINK], worldPos);
	ret->type = LINK;
	entities[App->sceneM->currentScene->currentSector].push_front(ret);
	App->game->playerId = ret->id = entities[App->sceneM->currentScene->currentSector].begin();
	
	return ret;
}

//Enemy factory (X and Y in Map Coordinates)
Enemy * j1EntityManager::CreateEnemy(int sector, ENEMY_TYPE type, int x, int y, vector<iPoint> patrolPoints)
{
	Enemy* ret = nullptr;

	switch (type)
	{
	case GREEN_SOLDIER:
		ret = new GreenSoldier();
		break;
	}

	iPoint worldPos = App->map->GetTileCenter(iPoint(x, y));

	ret->Spawn(dir[ENEMY], worldPos);
	ret->type = ENEMY;
	entities[App->sceneM->currentScene->currentSector].push_back(ret);
	ret->id = entities[App->sceneM->currentScene->currentSector].end();
	
	//Take patrol points from xml	
	ret->patrolPoints = patrolPoints;

	return nullptr;
}

Npc * j1EntityManager::CreateNPC(int sector, NPC_TYPE type , int x, int y, int id)
{
	Npc* ret = nullptr;

	switch (type)
	{
	case NPC_1:
		ret = new Npc1();
		break;
	}

	iPoint worldPos = App->map->GetTileCenter(iPoint(x, y));

	ret->Spawn(dir[NPC], worldPos, type);
	ret->type = NPC;
	entities[App->sceneM->currentScene->currentSector].push_back(ret);
	ret->id = entities[App->sceneM->currentScene->currentSector].end();
	
	//For dialog purposes
	ret->npcId = id;

	return nullptr;
}
