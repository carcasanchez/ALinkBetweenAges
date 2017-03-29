#include "Scene.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "Player.h"
#include "j1EntityManager.h"
#include "Enemy.h"

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

bool Scene::Load(const char* path)
{
	bool ret = true;

	pugi::xml_node data;
	pugi::xml_document	doc;
	char* buf;
	int size = App->fs->Load(path, &buf);
	pugi::xml_parse_result result = doc.load_buffer(buf, size);
	RELEASE(buf);

	if (!(ret = !(result == NULL)))
	{
		LOG("Could not load map xml file %s. pugi error: %s", path, result.description());
	}
	else
	{
		data = doc.child("scene");
		name.assign(data.attribute("name").as_string("Unnamed Scene"));
		std::string map = data.attribute("map").as_string("test.tmx");

		pugi::xml_node node = data.child("sectors");

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

		//Spawn Entities
		for (pugi::xml_node section = node.first_child(); section != NULL; section = section.next_sibling())
		{
			pugi::xml_node entities = section.child("entities");

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
					points);
			}

			entity = entities.child("special");
			for (pugi::xml_node special = entity.first_child(); special != NULL; special = special.next_sibling())
			{
				/* load otherEntity at maxSectors
				App->game->em->CreateNPC(
					maxSectors,
					SPECIAL_ENTITY_TYPE(other.attribute("type").as_int()),
					other.attribute("x").as_int(),
					other.attribute("y").as_int());*/
			}

			pugi::xml_node exits = section.child("exits");
			for (pugi::xml_node special = entity.first_child(); special != NULL; special = special.next_sibling())
			{
				/* add exits to entity manager*/
			}

			maxSectors++;
		}

		

		// TODO manage slot variations to scene
		//------------------------------------
	}

	return ret;
}

bool Scene::Update(float dt)
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	App->map->Draw();

	return ret;
}

bool Scene::CleanUp()
{
	currentSector = maxSectors = 1;
	return App->game->em->CleanUp();
}