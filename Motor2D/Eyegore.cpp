#include "j1App.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "Animation.h"
#include "Eyegore.h"
#include "j1EntityManager.h"
#include "Player.h"
#include "j1CollisionManager.h"
#include "Object.h"
#include <time.h>


bool Eyegore::Spawn(std::string file, iPoint pos)
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
		pugi::xml_node attributes = attributesFile.child("attributes").child("eyegore");

		LoadAttributes(attributes);

		enemyState = PATROLING;
		actionState = IDLE;

		attackRatio = attributes.child("ratios").attribute("attack").as_int(0);
		outFightRange = attributes.child("ranges").attribute("out").as_int(0);
		hostileRange = attributes.child("ranges").attribute("hostile").as_int(0);	
		rageSpeed = attributes.child("base").attribute("rage_speed").as_int(0);

		rageLife= attributes.child("base").attribute("rage_life").as_int(0);

		arrowWeakness = true;
		damagedLimit = 200;
	}
	return ret;
}

bool Eyegore::Update(float dt)
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


bool Eyegore::Chasing(float dt)
{
	int playerDistance = App->game->em->player->currentPos.DistanceTo(currentPos);
	if (playerDistance >= hostileRange)
	{
		enemyState = PATROLING;
		return true;
	}
	
	iPoint playerTile = App->map->WorldToMap(App->game->em->player->currentPos.x, App->game->em->player->currentPos.y);

	int currentSpeed;
	if (life <= rageLife)
		currentSpeed = rageSpeed;
	else currentSpeed = speed;
	GoTo(playerTile, currentSpeed, dt);
	LookToPlayer();

	return true;
}


bool Eyegore::StepBack(float dt)
{
	iPoint movement;

	if (App->game->em->player->currentPos.x > currentPos.x)
		movement.x = -1;
	else movement.x = +1;
	if (App->game->em->player->currentPos.y > currentPos.y)
		movement.y = -1;
	else movement.y = +1;

	Move(SDL_ceil(movement.x*speed*dt * 10), SDL_ceil(movement.y*speed*dt * 10));

	if (damagedTimer.ReadMs() > damagedLimit && pushedBackTimer.ReadMs() > 200)
	{
		enemyState = PATROLING;
		sprite->tint = { 255, 255, 255, 255 };
		if (life == 0)
		{
			life--;
		}
	}

	return true;
}