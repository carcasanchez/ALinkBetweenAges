#include "Tektite.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "Animation.h"
#include "j1EntityManager.h"
#include "Player.h"

bool Tektite::Spawn(std::string file, iPoint pos)
{
	bool ret = true;

	//set position
	currentPos = lastPos = pos;

	// load xml attributes
	pugi::xml_document	attributesFile;
	char* buff;
	int size = App->fs->Load(file.c_str(), &buff);
	pugi::xml_parse_result result = attributesFile.load_buffer(buff, size);
	RELEASE_ARRAY(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}

	else
	{
		pugi::xml_node attributes = attributesFile.child("attributes").child("tektite");

		LoadAttributes(attributes);

		enemyState = PATROLING;
		actionState = WALKING;
		hostileRange = attributes.child("ranges").attribute("hostile").as_int(0);
		outFightRange = attributes.child("ranges").attribute("out").as_int(0);
		

	}
	return ret;
}


bool Tektite::Update(float dt)
{

	switch (enemyState)
	{
	case(PATROLING):
		Patroling(dt);
		break;
	case(CHASING):
		Chasing(dt);
		break;
	case(STEP_BACK):
		StepBack(dt);
		break;
	}
	return true;
}

bool Tektite::Chasing(float dt)
{
	int playerDistance = App->game->em->player->currentPos.DistanceTo(currentPos);
	if (playerDistance >= hostileRange)
	{
		enemyState = PATROLING;
		return true;
	}


	iPoint playerTile = App->map->WorldToMap(App->game->em->player->currentPos.x, App->game->em->player->currentPos.y);
	GoTo(playerTile, speed, dt);
	

	return true;
}




void Tektite::OnDeath()
{
	toDelete = true;
	iPoint mapPos = App->map->WorldToMap(currentPos.x, currentPos.y);
	int drop = rand() % 5;
	if (drop == 3)
	{
		App->game->em->CreateObject(1, mapPos.x, mapPos.y, LIFEHEART);
	}
}