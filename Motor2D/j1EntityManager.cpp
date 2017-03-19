#include "j1EntityManager.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1GameLayer.h"
#include "p2Point.h"
#include "Entity.h"
#include "Player.h"
#include "Enemy.h"
#include "j1CollisionManager.h"

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
	std::list<Entity*>::iterator item = entities.begin();
	while (item != entities.end())
	{
		if ((*item)->life <= 0)
		{
			(*item)->OnDeath();
			if((*item)->toDelete)
				item = entities.erase(item); //calls destroyer
		}
		else
		{
			++item;
		}

		// reasign ids
		if(item != entities.end())
			(*item)->id = item;
	}

	return ret;
}

bool j1EntityManager::Update(float dt)
{
	bool ret = true;

	for (std::list<Entity*>::iterator item = entities.begin(); item != entities.end(); item++)
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

	for (std::list<Entity*>::iterator item = entities.begin(); item != entities.end(); item++)
	{
		(*item)->Draw();
	}

	return ret;
}

bool j1EntityManager::CleanUp()
{
	bool ret = true;

	for (std::list<Entity*>::iterator item = entities.begin(); item != entities.end(); item++)
	{
		entities.erase(item);
	}

	return ret;
}

Player* j1EntityManager::CreatePlayer(int x, int y)
{
	Player* ret = new Player();

	ret->Spawn(dir[LINK], iPoint(x,y));
	ret->type = LINK;
	entities.push_front(ret);
	App->game->playerId = ret->id = entities.begin();
	
	return ret;
}

//Enemy factory
Enemy * j1EntityManager::CreateEnemy(ENEMY_TYPE type, int x, int y)
{
	Enemy* ret = nullptr;

	switch (type)
	{
	case GREEN_SOLDIER:
		ret = new GreenSoldier();
		break;
	}

	ret->Spawn(dir[ENEMY], iPoint(x, y));
	ret->type = ENEMY;
	entities.push_back(ret);
	ret->id = entities.end();

	return nullptr;
}
