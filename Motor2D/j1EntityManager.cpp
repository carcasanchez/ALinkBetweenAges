#include "j1EntityManager.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1GameLayer.h"
#include "Entity.h"
#include "Player.h"
#include "Enemy.h"
#include "Object.h"
#include "NPC.h"
#include "j1CollisionManager.h"
#include "j1SceneManager.h"
#include "Scene.h"
#include "j1Map.h"
#include "p2Defs.h"

//=====Enemy Includes
#include "GreenSoldier.h"
#include "RedSoldier.h"
#include "Octorok.h"
#include "DarkZelda.h"
#include "Wizdrove.h"
#include "Eyegore.h"
#include "Tektite.h"


j1EntityManager::j1EntityManager() : player(NULL), sector(NULL)
{}

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

	if (player->life <= 0)
		player->OnDeath();

	// check for dead entities

	std::vector<Entity*>::iterator item = entities[1].begin();
	while (item != entities[1].end())
	{
		if ((*item)->life < 0 && (*item)->dead == false)
		{
			(*item)->OnDeath();
			if ((*item)->toDelete)
			{
				if ((*item)->col)
				{
					(*item)->col->active = false;
				}
				(*item)->currentPos = {-1000, -1000};
				(*item)->UpdateCollider();
				(*item)->dead = true;
			}
				
		}
		else
		{
			++item;
		}
	}

	return ret;
}

bool j1EntityManager::Update(float dt)
{
	bool ret = true;

	if (!player->sceneOverride)
	{
		player->Update(dt);
		player->UpdateCollider();
	}
	

	for (std::vector<Entity*>::iterator item = entities[*sector].begin(); item != entities[*sector].end(); item++)
	{		

		if (!(*item))
			continue;
		if (!(*item)->dead)
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

	if (!player->sceneOverride)
	{
		player->Draw();
	}

	for (std::vector<Entity*>::iterator item = entities[*sector].begin(); item != entities[*sector].end(); item++)
	{
		if (!(*item))
			continue;
		(*item)->Draw();
	}

	return ret;
}

bool j1EntityManager::CleanUp()
{
	bool ret = true;

	RELEASE(player);
	CleanEntities();

	return ret;
}

Player* j1EntityManager::CreatePlayer(int x, int y, LINK_AGE age)
{
	Player* ret = new Player();


	iPoint worldPos = App->map->GetTileCenter(iPoint(x, y));
	ret->age = age;

	if (!ret->Spawn(dir[LINK], worldPos))
	{
		RELEASE(ret);
		ret = NULL;
	}
	
	return ret;
}

//Entity factory methods (X and Y in Map Coordinates)
Enemy * j1EntityManager::CreateEnemy(int sector, ENEMY_TYPE type, int x, int y, vector<iPoint> patrolPoints, int id)
{
	Enemy* ret = nullptr;

	switch (type)
	{
	case GREEN_SOLDIER:
		ret = new GreenSoldier();
		break;
	case RED_SOLDIER:
		ret = new RedSoldier();
		break;
	case DARK_ZELDA:
		ret = new DarkZelda();
		break;
	case OCTOROK:
		ret = new Octorok();
		break;
	case WIZDROVE:
		ret = new Wizdrove();
		break;
	case EYEGORE:
		ret = new Eyegore();
		break;
	case TEKTITE:
		ret = new Tektite();
		break;
	}

	iPoint worldPos = App->map->GetTileCenter(iPoint(x, y));

	if (ret->Spawn(dir[ENEMY], worldPos))
	{
		ret->type = ENEMY;
		entities[sector].push_back(ret);
		ret->id = id;
		ret->patrolPoints = patrolPoints;
	}
	else
	{
		RELEASE(ret);
		ret = NULL;
	}

	return ret;
}

Npc * j1EntityManager::CreateNPC(int sector, NPC_TYPE type , int x, int y, int id)
{
	Npc* ret = NULL;

	switch (type)
	{
	case NPC_ZELDA:
		ret = new Npc_Zelda();
		break;
	case NPC_UNCLE:
		ret = new Npc_Uncle();
		break;
	case NPC_HIPPIE:
		ret = new Npc_Hippie();
		break;
	case NPC_FARM_KID:
		ret = new Npc_Farm_Kid();
		break;
	case NPC_WOMAN:
		ret = new Npc_Woman();
		break;
	case NPC_GRANDMA:
		ret = new Npc_Grandma();
		break;
	case NPC_TAVERN_OLD_MAN:
		ret = new Npc_Tavern_Old_Man();
		break;
	case NPC_LITTLE_KID:
		ret = new Npc_Little_Kid();
		break;
	case NPC_GANON:
		ret = new Npc_Ganon();
		break;
	case SIGN:
		ret = new Npc_Sign();
		break;
	case NPC_SOLDIER:
		ret = new Npc_Soldier();
		break;
	case NPC_DARK_ZELDA:
		ret = new Npc_Dark_Zelda();
		break;
	default:
		return nullptr;
	}

	iPoint worldPos = App->map->GetTileCenter(iPoint(x, y));

	if (ret->Spawn(dir[NPC], worldPos, type))
	{
		ret->type = NPC;
		entities[sector].push_back(ret);
		ret->id = id;
	}
	else
	{
		RELEASE(ret);
		ret = NULL;
	}

	return ret;
}

Object * j1EntityManager::CreateObject(int sector, int x, int y, OBJECT_TYPE type, int id, int direction)
{
	Object* ret;
	switch (type)
	{
	default:
		ret = new Object;
		break;

	case LINK_ARROW:
		ret = new Arrow;
		break;

	case OCTO_STONE:
		ret = new Octostone;
		break;

	case GREEN_RUPEE:
	case BLUE_RUPEE:
	case RED_RUPEE:
		ret = new Rupee;
		break;

	case CHEST:
		ret = new Chest;
		break;
	case MAGIC_SLASH:
		ret = new MagicSlash;
		break;

	case BOMB:
		ret = new Bomb;
		break;

	case ZELDA_ARROW:
		ret = new ZeldaArrow;
		break;
	case BOMB_EXPLOSION:
		ret = new BombExplosion;
		break;
	case FALLING_BOLT:
		ret = new FallingBolt;
		break;
	case BOW:
		ret = new Bow;
		break;
	case STAMINA_POTION:
		ret = new StaminaPotion;
		break;
	case PILLAR:
		ret = new Pillar;
		break;
	case INTERRUPTOR:
		ret = new Interruptor;
		break;
	case BUSH:
		ret = new Bush;
		break;
	}
	

	iPoint worldPos = App->map->MapToWorld(x, y);

	if (ret->Spawn(dir[OBJECT], worldPos, type, (DIRECTION)direction))
	{
		ret->type = OBJECT;
		entities[sector].push_back(ret);
		ret->id = id;
		ret->UpdateCollider();
	}
	


	else 
	{
		RELEASE(ret);
		ret = NULL;
	}

	return ret;
}

Object * j1EntityManager::CreateDeadObject(int sector, int x, int y, OBJECT_TYPE type, int id, int direction)
{
	Object* ret;
	switch (type)
	{
	default:
		ret = new Object;
		break;

	case LINK_ARROW:
		ret = new Arrow;
		break;

	case OCTO_STONE:
		ret = new Octostone;
		break;

	case GREEN_RUPEE:
	case BLUE_RUPEE:
	case RED_RUPEE:
		ret = new Rupee;
		break;

	case CHEST:
		ret = new Chest;
		break;
	case MAGIC_SLASH:
		ret = new MagicSlash;
		break;

	case BOMB:
		ret = new Bomb;
		break;

	case ZELDA_ARROW:
		ret = new ZeldaArrow;
		break;
	case BOMB_EXPLOSION:
		ret = new BombExplosion;
		break;
	case FALLING_BOLT:
		ret = new FallingBolt;
		break;
	case BOW:
		ret = new Bow;
		break;
	case STAMINA_POTION:
		ret = new StaminaPotion;
		break;
	case PILLAR:
		ret = new Pillar;
		break;
	case INTERRUPTOR:
		ret = new Interruptor;
		break;
	case BUSH:
		ret = new Bush;
		break;
	case SWORD_BOLT:
		ret = new SwordBolt;
		break;
	case DEATH_CLOUD:
		ret = new DeathCloud;
		break;
	}


	iPoint worldPos = App->map->MapToWorld(x, y);

	if (ret->Spawn(dir[OBJECT], worldPos, type, (DIRECTION)direction))
	{
		ret->type = OBJECT;
		entities[sector].push_back(ret);
		ret->id = id;
		ret->UpdateCollider();
		ret->col->active = false;
		ret->life = -1;
		ret->dead = true;
	}



	else
	{
		RELEASE(ret);
		ret = NULL;
	}

	return ret;
}

Object * j1EntityManager::ActiveObject(int x, int y, OBJECT_TYPE type, int dir)
{
	vector<Entity*>::iterator it;
	for (it = entities[1].begin(); it != entities[1].end(); it++)
	{
		if ((*it)->type != OBJECT)
			continue;
		if(((Object*)(*it))->objectType==type && (*it)->dead)
		{
			(*it)->currentPos = {x, y};
			(*it)->dead = false;
			(*it)->life = 1;
			(*it)->currentDir = (DIRECTION)dir;
			if ((*it)->col)
				(*it)->col->active = true;
			(*it)->UpdateCollider();

			if (type == BOMB)
			{
				((Bomb*)(*it))->explode_counter.Start();
			}
			if (type == BOMB_EXPLOSION)
			{
				((BombExplosion*)(*it))->dead_counter.Start();
			}

			return ((Object*)(*it));
		}
	}
	return nullptr;
}

Entity * j1EntityManager::GetEntityFromId(int id)
{
	for (int i = 1; i <= App->sceneM->currentScene->maxSectors; i++)
	{
		for (std::vector<Entity*>::iterator item = entities[i].begin(); item != entities[i].end(); item++)
		{
			if (!(*item))
				continue;
			if((*item)->id == id)
				return (*item);			
		}

	}

	return nullptr;
}

bool j1EntityManager::CleanEntities()
{
	constantEntityIndex = 0;

	std::vector<Entity*> keepAlive;

	std::vector<Entity*>::iterator item;
	for (item = entities[1].begin(); item != entities[1].end(); item++)
		if ((*item)->keepExisting)
		{
			(*item)->keepExisting = false;
			keepAlive.push_back((*item));
		}
		else
		{

			RELEASE(*item);
		}

	entities[1].clear();
	entities.clear();

	for (std::vector<Entity*>::iterator it = keepAlive.begin(); it != keepAlive.end(); it++)
	{
		entities[1].push_back((*it));
	}

	return true;
}

bool j1EntityManager::CleanTempEntities()
{
	bool ret = true;
	std::vector<Entity*> keepAlive;


	std::vector<Entity*>::iterator item = entities[1].begin();

	for (int i = 0; i < constantEntityIndex;i++, item++){}

	for (; item != entities[1].end(); item++)
	{
		if ((*item)->keepExisting)
		{
			(*item)->keepExisting = false;
			keepAlive.push_back((*item));
		}
		else
		{
			RELEASE(*item);
		}
	}

	entities[1].resize(122);


	for (std::vector<Entity*>::iterator it = keepAlive.begin(); it != keepAlive.end(); it++)
	{
		entities[1].push_back((*it));
	}


	return ret;
}

void j1EntityManager::SetSectorRef(int*s)
{
	sector = s;
}