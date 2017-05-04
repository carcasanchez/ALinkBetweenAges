#include "Scene.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "Entity.h"
#include "j1EntityManager.h"
#include "Enemy.h"
#include "Exit.h"
#include "j1Audio.h"
#include "Player.h"
#include "Object.h"
#include "j1Render.h"
#include "j1QuestManager.h"
#include "p2Log.h"

Scene::Scene()
{
	name.assign("Unnamed Scene");
	currentSector = maxSectors = 1;
}

Scene::Scene(const char* path)
{
	name.assign(path);
	currentSector = maxSectors = 1;
}

bool Scene::Load(const char* path, const bool reloadMap)
{
	bool ret = true;

	pugi::xml_node data;
	pugi::xml_document	doc;
	char* buf;
	int size = App->fs->Load(path, &buf);
	pugi::xml_parse_result result = doc.load_buffer(buf, size);
	RELEASE_ARRAY(buf);

	if (!(ret = !(result == NULL)))
	{
		LOG("Could not load map xml file %s. pugi error: %s", path, result.description());
	}
	else
	{
		data = doc.child("scene");
		name.assign(data.attribute("name").as_string("Unnamed Scene"));
		std::string map = data.attribute("map").as_string("none");

		pugi::xml_node node = data.child("sectors");

		if (reloadMap)
		{
			//IMPORTANT: CREATE THE WALKABILITY MAP BEFORE SPAWN ENTITIES
			if (ret = (App->map->Load(map.c_str())))
			{
				int w, h;
				uchar* player_data = NULL;
				uchar* enemy_data = NULL;

				if (App->map->CreateWalkabilityMap(w, h, &player_data, &enemy_data))
				{
					App->pathfinding->SetPlayerMap(w, h, player_data);
					App->pathfinding->SetEnemyMap(w, h, enemy_data);
				}

				RELEASE_ARRAY(player_data);
				RELEASE_ARRAY(enemy_data);
			}
		}

		

		//Spawn Entities
		for (pugi::xml_node section = node.first_child(); section != NULL; section = section.next_sibling())
		{
			pugi::xml_node entities = section.child("entities");


			//Npc
			pugi::xml_node entity = entities.child("npcs");
			for (pugi::xml_node npc = entity.first_child(); npc != NULL; npc = npc.next_sibling())
			{
				/* load npc at maxSectors*/
				App->game->em->CreateNPC(
					maxSectors,
					NPC_TYPE(npc.attribute("type").as_int()),
					npc.attribute("x").as_int(),
					npc.attribute("y").as_int(),
					npc.attribute("id").as_int());
			}

			//Enemies
			entity = entities.child("enemies");
			for (pugi::xml_node enemy = entity.first_child(); enemy != NULL; enemy = enemy.next_sibling())
			{
				vector<iPoint> points;
				for (pugi::xml_node patrolPoint = enemy.child("patrol"); patrolPoint; patrolPoint = patrolPoint.next_sibling("patrol"))
				{
					points.push_back(iPoint(patrolPoint.attribute("x").as_int(), patrolPoint.attribute("y").as_int()));
				}

				App->game->em->CreateEnemy(
					maxSectors,
					ENEMY_TYPE(enemy.attribute("type").as_int()),
					enemy.attribute("x").as_int(),
					enemy.attribute("y").as_int(),
					points,
					enemy.attribute("id").as_int(-1));
			}

			//Items and objects
			entity = entities.child("special");
			for (pugi::xml_node special = entity.first_child(); special != NULL; special = special.next_sibling())
			{
		
				Object* obj = App->game->em->CreateObject(
					maxSectors,
					special.attribute("x").as_int(),
					special.attribute("y").as_int(), 
					(OBJECT_TYPE)(special.attribute("object_type").as_int()),
					special.attribute("id").as_int(-1));

				if (obj->objectType == CHEST)
				{
					((Chest*)obj)->objectInside = (OBJECT_TYPE)special.attribute("item_inside").as_int();
				}


			}


			//Exits
			pugi::xml_node exits = section.child("exits");
			for (pugi::xml_node exit = exits.first_child(); exit != NULL; exit = exit.next_sibling())
			{

				iPoint worldPos = App->map->MapToWorld(exit.attribute("x").as_int(), exit.attribute("y").as_int());

				SDL_Rect rect = {
					worldPos.x,
					worldPos.y,
					exit.attribute("w").as_int() * 8,
					exit.attribute("h").as_int() * 8};

				Exit* tmp = new Exit();

				if (tmp->Spawn(
					exit.attribute("destinyScene").as_string(),
					exit.attribute("destinyExit").as_int(),
					rect,
					DIRECTION(exit.attribute("direction").as_int())))
				{
					this->exits[maxSectors].push_back(tmp);
				}
			}

			//Quests
			App->quest->LoadQuests(section.child("quests"));

			//Music
			App->audio->PlayMusic(section.child("music").attribute("file").as_string());

			if (section.child("camera").attribute("locked").as_bool())
				App->render->cameraLocked = true;

			
			else App->render->cameraLocked = false;

			maxSectors++;
		}

		

		

		// TODO manage slot variations to scene
		/*------------------------------------

		- locked doors

		*/

		App->game->em->SetSectorRef(&currentSector);
	}

	return ret;
}

bool Scene::Update(float dt)
{
	bool ret = true;

	std::list<Exit*>::iterator exit = exits[currentSector].begin();
	for (; exit != exits[currentSector].cend() && ret; exit++)
		(*exit)->Update();


	exit = exits[currentSector].begin();
	for (; exit != exits[currentSector].cend() && ret; exit++)
		(*exit)->Draw();

	return ret;
}

bool Scene::CleanUp()
{
	bool ret = true;

	currentSector = maxSectors = 1;

	for (int sector = 1; sector <= maxSectors; sector++)
	{
		for (std::list<Exit*>::iterator exit = exits[sector].begin(); exit != exits[sector].end(); exit++)
		{
			delete (*exit);
			exits[sector].erase(exit);
		}

		exits[sector].clear();
	}

	exits.clear();

	return ret;
}

iPoint Scene::GetExitPlayerPos(int sector, int exitNum, DIRECTION &destDir)
{
	iPoint ret = { 0,0 };

	std::list<Exit*>::iterator it = exits[sector].begin();

	for (int i = 0; i < exitNum && it != exits[sector].end(); i++)
		it++;

	ret.x = (*it)->rect.x + (*it)->rect.w/2;
	ret.y = (*it)->rect.y + (*it)->rect.h / 2;

	destDir = (*it)->dir;

	return ret;
}