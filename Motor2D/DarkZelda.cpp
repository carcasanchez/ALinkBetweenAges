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
#include "j1SceneManager.h"
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
		fightRange = 50;
		dodgeSpeed = 200;
		dodgeLimit = 200;
	
	}
	return ret;
}

void DarkZelda::OnDeath()
{
	if(phase == 1)
	{
		keepExisting = true;
		App->sceneM->RequestSceneChange(App->map->MapToWorld(75, 35), "outsideCastle", D_UP);
	
		currentPos = App->map->MapToWorld(75, 25);
		sprite->tint = { 255, 255, 255, 255 };
		phase = 2;
		life = 75;	
		enemyState = KEEP_DISTANCE;

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
		case KEEP_DISTANCE:
			KeepDistance(dt);
			break;
		case STEP_BACK:
			StepBack(dt);
			break;
		case CHASING:
			Chasing(dt);
			break;
		case DODGING_LINK:
			Dodging(dt);
			break;
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




bool DarkZelda::KeepDistance(float dt)
{
	LookToPlayer();

	actionState = WALKING;

	//Choose randomly if the flanking direction changes
	if (abs(App->game->em->player->currentPos.x - currentPos.x) == abs(App->game->em->player->currentPos.y - currentPos.y))
	{
		srand(time(NULL));
		bool change = rand() % 2;
		if (change)
		{
			flankingDir = !flankingDir;
		}
	}

	//Choose if attack or not if Link is aligned to enemy
	/*if (attackTimer.ReadMs() > attackRatio)
	{
		srand(time(NULL));
		bool attack = rand() % 2;
		if (abs(currentPos.y - App->game->em->player->currentPos.y)<3 || abs(currentPos.x - App->game->em->player->currentPos.x)<3)
		{
			attackTimer.Start();
			enemyState = CHARGING;
		}
	}*/


	if (App->game->em->player->currentPos.DistanceTo(currentPos) > fightRange*1.5)
	{
		enemyState = CHASING;
	}

	

	//Movement depending on the player relative direction
	iPoint flankingMovement;
	if (flankingDir)
	{
		switch (currentDir)
		{
		case D_UP:
			flankingMovement = { 1, 0 };
			break;
		case D_DOWN:
			flankingMovement = { -1, 0 };
			break;
		case D_RIGHT:
			flankingMovement = { 0, 1 };
			break;
		case D_LEFT:
			flankingMovement = { 0, -1 };
			break;
		}
	}
	else
		switch (currentDir)
		{
		case D_UP:
			flankingMovement = { -1, 0 };
			break;
		case D_DOWN:
			flankingMovement = { 1, 0 };
			break;
		case D_RIGHT:
			flankingMovement = { 0, -1 };
			break;
		case D_LEFT:
			flankingMovement = { 0, 1 };
			break;
		}



	//Change the flanking direction if the enemy hits something
	if (Move(SDL_ceil(speed*dt)*flankingMovement.x, SDL_ceil(speed*dt)*flankingMovement.y) == false)
	{
		flankingDir = !flankingDir;
	}




	return true;
}

//Persecute Player
bool DarkZelda::Chasing(float dt)
{
	int playerDistance = App->game->em->player->currentPos.DistanceTo(currentPos);

	 if (playerDistance <= fightRange)
	{
		enemyState = KEEP_DISTANCE;
		path.clear();
		attackTimer.Start();
		srand(time(NULL));
		flankingDir = rand() % 2;
	}

	iPoint playerTile = App->map->WorldToMap(App->game->em->player->currentPos.x, App->game->em->player->currentPos.y);

	GoTo(playerTile, speed, dt);
	LookToPlayer();

	return true;
}

bool DarkZelda::Dodging(float dt)
{
	iPoint movement;
	actionState = WALKING;
	LookToPlayer();

	if (flankingDir)
	{
		switch (currentDir)
		{
		case D_UP:
			movement = { 1, 0 };
			break;
		case D_DOWN:
			movement = { -1, 0 };
			break;
		case D_RIGHT:
			movement = { 0, 1 };
			break;
		case D_LEFT:
			movement = { 0, -1 };
			break;
		}
	}
	else
		switch (currentDir)
		{
		case D_UP:
			movement = { -1, 0 };
			break;
		case D_DOWN:
			movement = { 1, 0 };
			break;
		case D_RIGHT:
			movement = { 0, -1 };
			break;
		case D_LEFT:
			movement = { 0, 1 };
			break;
		}

	


		
	Move(SDL_ceil(movement.x*dodgeSpeed*dt), SDL_ceil(movement.y*dodgeSpeed*dt));

	if (dodgeTimer.ReadMs() > dodgeLimit)
	{
		enemyState = CHASING;
	}

	return false;
}

bool DarkZelda::GetHit()
{
	switch (phase)
	{
	case 1:
		life -= App->game->em->player->damage;
		sprite->tint = { 255, 150, 150, 255 };
		enemyState = STEP_BACK;
		damagedTimer.Start();
		break;

	case 2:
		enemyState = DODGING_LINK;
		srand(time(NULL));
		bool change = rand() % 2;
		if (change)
		{
			flankingDir = true;
		}
		else flankingDir = false;
		dodgeTimer.Start();
		break;
	}

	return true;
}

bool DarkZelda::StepBack(float dt)
{
	iPoint movement;

	if (App->game->em->player->currentPos.x > currentPos.x)
		movement.x = -1;
	else movement.x = +1;
	if (App->game->em->player->currentPos.y > currentPos.y)
		movement.y = -1;
	else movement.y = +1;

	Move(SDL_ceil(movement.x*speed*dt * 5), SDL_ceil(movement.y*speed*dt * 5));

	if (damagedTimer.ReadMs() > damagedLimit && pushedBackTimer.ReadMs() > 200)
	{
		enemyState = KEEP_DISTANCE;
		sprite->tint = { 255, 255, 255, 255 };
		if (life == 0)
		{
			life--;
		}
	}

	return true;
}
