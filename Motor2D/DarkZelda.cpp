#include "DarkZelda.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "Animation.h"
#include "j1EntityManager.h"
#include "Object.h"
#include "j1GameLayer.h"
#include "Player.h"
#include <time.h>

bool DarkZelda::Spawn(std::string file, iPoint pos)
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
		pugi::xml_node attributes = attributesFile.child("attributes").child("dark_zelda");

		LoadAttributes(attributes);

		enemyState = LATERAL_WALK;
		actionState = WALKING;

		speed = 10;
		walkTimelimit = 500;

		bowTenseLimit = 2000;
	}
	return ret;
}

void DarkZelda::OnDeath()
{
	if(phase == 1)
	{
		sprite->tint = { 255, 255, 255, 255 };
		phase = 2;
		life = 5;	
		enemyState = PATROLING;
	}

	else if (phase == 2)
	{
		toDelete = true;
	}

}



bool DarkZelda::Update(float dt)
{	

	iPoint playerTile = App->map->WorldToMap(App->game->em->player->currentPos.x, App->game->em->player->currentPos.y);

	switch (phase)
	{	
	case 1:
		switch (enemyState)
		{
		case PATROLING:
			Patroling(dt);
			break;
		case LATERAL_WALK:
			LateralWalk(dt);
			break;
		case CHARGE_BOW:
			ChargeBow(dt);
			break;
		case STEP_BACK:
			StepBack(dt);
			break;
		}
		break;
	case 2:
		switch (enemyState)
		{

		}
		break;
	case 3:
		switch (enemyState)
		{

		}
		break;
	}
	
	return false;
}


//Zelda State machine

bool DarkZelda::LateralWalk(float dt)
{
	currentDir = D_DOWN;
	actionState = WALKING;

	bool ret;
	if (currentPos.x < App->game->em->player->currentPos.x)
	{
		ret = Move(SDL_ceil(speed*dt), 0);
		
	}
	else if(currentPos.x > App->game->em->player->currentPos.x)
		ret = Move(-SDL_ceil(speed*dt), 0);



	if (walkTimer.ReadMs() > walkTimelimit && abs (App->game->em->player->currentPos.x- currentPos.x) < 10 )
	{
		enemyState = CHARGE_BOW;
		chargeBowTimer.Start();
	}

	return false;
}

bool DarkZelda::ChargeBow(float dt)
{
	actionState = SHOOTING_BOW;

	if (currentAnim->isOver())
	{
		iPoint mapPos = App->map->WorldToMap(currentPos.x, currentPos.y);
		App->game->em->CreateObject(1, mapPos.x, mapPos.y, ZELDA_ARROW);
		currentAnim->Reset();
		enemyState = LATERAL_WALK;
		walkTimer.Start();
	}

	return false;
}
