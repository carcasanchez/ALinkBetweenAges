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
#include "j1CollisionManager.h"
#include <time.h>
#include <math.h>

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

		phase = 1;
		speed = 50;
		walkTimelimit = 500;
		fightRange = 80;
		dodgeSpeed = 200;
		dodgeLimit = 300;
		attackRatio = 1500;
		attackRatio_2 = 1000;
		attackSpeed = 200;

		holdPosLimit = 800;
		holdStabLimit = 800;

		teleportRange = 100;
		chargeTime = 300;

		rageLife = 10;
		rageLimit = 3000;

		damagedLimit = 100;
	
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
		life = 20;	
		enemyState = KEEP_DISTANCE;
		attackTimer.Start();

	}

	else if (phase == 2)
	{
		currentPos = App->map->MapToWorld(75, 25);
		sprite->tint = { 255, 255, 255, 255 };
		phase = 3;
		life = 20;
		attackRatio = attackRatio_2;
		enemyState = KEEP_DISTANCE;
		attackTimer.Start();
	}

	else if (phase == 3)
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
		case CHARGING:
			Charging(dt);
			break;
		case THROWING_ATTACK:
			Attack(dt);
			break;
		case TELEPORT:
			TeleportAndAttack(dt);
			break;
		case FRONTAL_ATTACK:
			Stab(dt);
			break;
		case CIRCULAR_ATTACK:
			Spin(dt);
			break;
		}
		break;
	case 3:
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
		case CHARGING:
			Charging(dt);
			break;
		case THROWING_ATTACK:
			Attack(dt);
			break;
		case TELEPORT:
			TeleportAndAttack(dt);
			break;
		case FRONTAL_ATTACK:
			Stab(dt);
			break;
		case CIRCULAR_ATTACK:
			Spin(dt);
			break;
		}
		break;
	}
	
	return false;
}


//Zelda State machine

//Phase 1
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


//Phase 2 & 3

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
			flankingDir = !flankingDir;
	}
	
	//Choose if attack or stab if Link is aligned to enemy
	if (attackTimer.ReadMs() > attackRatio)
	{
		if (abs(currentPos.y - App->game->em->player->currentPos.y) < 24 || abs(currentPos.x - App->game->em->player->currentPos.x) < 24)
		{
			SetAttack();
		}
	}

	if (App->game->em->player->currentPos.DistanceTo(currentPos) > fightRange*1.5)
	{
		enemyState = CHASING;
	}	


	if (rage)
	{
		enemyState = TELEPORT;
		actionState = DISAPPEARING;
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

bool DarkZelda::Chasing(float dt)
{
	int playerDistance = App->game->em->player->currentPos.DistanceTo(currentPos);

	iPoint playerTile = App->map->WorldToMap(App->game->em->player->currentPos.x, App->game->em->player->currentPos.y);

	GoTo(playerTile, speed, dt);
	LookToPlayer();

	 if (playerDistance <= fightRange)
	{
		enemyState = KEEP_DISTANCE;
		path.clear();
		attackTimer.Start();
		srand(time(NULL));
		flankingDir = rand() % 2;
	}

	 if (attackTimer.ReadMs() > attackRatio)
	 {
		 if (abs(currentPos.y - App->game->em->player->currentPos.y) < 32 || abs(currentPos.x - App->game->em->player->currentPos.x) < 32)	 
				 SetAttack();
	 }

	  if (playerDistance > teleportRange || rage == true)
	 {
		 enemyState = TELEPORT;
		 actionState = DISAPPEARING;
	 }


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

bool DarkZelda::Charging(float dt)
{
	actionState = PREPARING_SWORD;
	
	bool stop = false;
	bool ret = true;

	switch (currentDir)
	{
	case D_UP:
		ret = Move(0, -SDL_ceil(dt*attackSpeed));
		if (App->map->WorldToMap(currentPos.x, currentPos.y).y < lastPlayerPos.y)
			stop = true;
		break;
	case D_DOWN:
		ret = Move(0, SDL_ceil(dt*attackSpeed));
		if (App->map->WorldToMap(currentPos.x, currentPos.y).y > lastPlayerPos.y)
			stop = true;
		break;
	case D_RIGHT:
		ret = Move(SDL_ceil(dt*attackSpeed), 0);
		if (App->map->WorldToMap(currentPos.x, currentPos.y).x > lastPlayerPos.x)
			stop = true;
		break;
	case D_LEFT:
		ret = Move(-SDL_ceil(dt*attackSpeed), 0);
		if (App->map->WorldToMap(currentPos.x, currentPos.y).x < lastPlayerPos.x)
			stop = true;
		break;
	}

	if (stop || ret == false)

	{
		enemyState = THROWING_ATTACK;
		switch (phase)
		{
		case 2:
			actionState = ATTACKING;
			break;
		case 3:
			actionState = SPECIAL_ATTACK;
			break;
		}
	}
	
	return true;
}

bool DarkZelda::Attack(float dt)
{
	if (actionState != HOLDING_NORMAL_SWORD && actionState != HOLDING_FIRE_SWORD)
	{
		if (currentAnim->isOver())
		{
			currentAnim->Reset();
			holdPosTimer.Start();
			if (rage)
			{
				if (rageTimer.ReadMs() > rageLimit)
					rage = false;
				else
				{ 
					actionState = WALKING;
					enemyState = CHASING;
					attackTimer.Start();
				}
			}
			else
			switch (phase)
			{
			case 2:
				actionState = HOLDING_NORMAL_SWORD;	
				break;
			case 3:
				actionState = HOLDING_FIRE_SWORD;
				break;
			}
			
		}
	}

	else 
	{
		if (holdPosTimer.ReadMs() > holdPosLimit)
		{
			actionState = WALKING;
			enemyState = CHASING;
			attackTimer.Start();
		}
	}

	return true;
}

bool DarkZelda::Stab(float dt)
{
	
	if (currentAnim->CurrentFrame() > 2 && !holdStab)
	{
		bool stop = false;
		bool ret = true;

		if (attackTimer.ReadMs() > chargeTime)
			stop = true;
		switch (currentDir)
		{
		case D_UP:
			ret = Move(0, -SDL_ceil(dt*attackSpeed));
			break;
		case D_DOWN:
			ret = Move(0, SDL_ceil(dt*attackSpeed));
			break;
		case D_RIGHT:
			ret = Move(SDL_ceil(dt*attackSpeed), 0);
			break;
		case D_LEFT:
			ret = Move(-SDL_ceil(dt*attackSpeed), 0);
			break;
		}
		
		if (stop || ret == false)
		{
			holdStab = true;
			holdStabTimer.Start();

			if (phase == 3)
			{
				iPoint mapPos = App->map->WorldToMap(currentPos.x, currentPos.y);
				bolt = App->game->em->CreateObject(1, mapPos.x, mapPos.y, SWORD_BOLT);
				bolt->currentDir = currentDir;
				switch (bolt->currentDir)
				{
				case D_DOWN:
					bolt->colPivot.y = 0;
					break;
				case D_RIGHT:
					bolt->colPivot.y = bolt->colPivot.x;
					bolt->colPivot.x = 0;
					break;
				case D_LEFT:
					int tmp = bolt->colPivot.x;
					bolt->colPivot.x = bolt->colPivot.y;
					bolt->colPivot.y = tmp;
					break;
				}


				if (currentDir == D_RIGHT || currentDir == D_LEFT)
				{
					int tmp = bolt->col->rect.h;
					bolt->col->rect.h = bolt->col->rect.w;
					bolt->col->rect.w = tmp;
				}
			}
		}
	}

	else if (holdStab)
	{
		if (holdStabTimer.ReadMs() > holdStabLimit)
		{
			currentAnim->Reset();
			enemyState = KEEP_DISTANCE;
			actionState = WALKING;
			attackTimer.Start();
			holdStab = false;
		}
	}


	return true;
}

bool DarkZelda::TeleportAndAttack(float dt)
{
	if (actionState == DISAPPEARING)
	{
		if (currentAnim->isOver())
		{
			currentAnim->Reset();
			actionState = APPEARING;
			currentPos = App->game->em->player->currentPos;
			switch (currentDir)
			{
			case D_UP:
				currentPos.y += 24;
				break;
			case D_DOWN:
				currentPos.y -= 24;
				break;
			case D_LEFT:
				currentPos.x += 24;
				break;
			case D_RIGHT:
				currentPos.x -= 24;
				break;
			}			
		}
	}

	else if (actionState == APPEARING)
	{
		if (currentAnim->isOver())
		{
			currentAnim->Reset();
			lastPlayerPos = App->map->WorldToMap(App->game->em->player->currentPos.x, App->game->em->player->currentPos.y);
			enemyState = CHARGING;
		}
	}

	return true;
}

bool DarkZelda::Spin(float dt)
{
	if (currentAnim->isOver())
	{
		currentAnim->Reset();
		enemyState = KEEP_DISTANCE;
		attackTimer.Start();
	}

	return true;
}




void DarkZelda::SetAttack()
{
	srand(time(NULL));
	int attack = rand() % 6;
	if (attack == 1 || attack == 0)
	{
		lastPlayerPos = App->map->WorldToMap(App->game->em->player->currentPos.x, App->game->em->player->currentPos.y);
		enemyState = CHARGING;
	}
	else if(attack == 3 || attack == 2)
	{
		attackTimer.Start();
		enemyState = FRONTAL_ATTACK;

		switch (phase)
		{
		case 2:
			actionState = STABBING;
			break;
		case 3:
			actionState = SPECIAL_STAB;
			break;

		}
	}
	else if (App->game->em->player->currentPos.DistanceTo(currentPos) < 40)
	{
		currentDir = D_DOWN;
		enemyState = CIRCULAR_ATTACK;
		actionState = SPINNING;
	}

	

	
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

	case 3:
		if(enemyState == KEEP_DISTANCE)
			{ 
			enemyState = DODGING_LINK;
			srand(time(NULL));
			bool change = rand() % 2;
			if (change)
			{
				flankingDir = true;
			}
			else flankingDir = false;
			dodgeTimer.Start();
		}
		else if(enemyState != CHARGING && enemyState != SPINNING)
		{
			currentAnim->Reset();
			life -= App->game->em->player->damage;
			sprite->tint = { 255, 0, 0, 255 };
			enemyState = STEP_BACK;
			actionState = IDLE;
			damagedTimer.Start();
		}
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

	if (damagedTimer.ReadMs() > damagedLimit && pushedBackTimer.ReadMs() > 100)
	{
		enemyState = KEEP_DISTANCE;
		sprite->tint = { 255, 255, 255, 255 };
		if (life == 0)
		{
			life--;
		}
	}

	if (phase == 3 && life <= rageLife)
	{
		rage = true;
		rageTimer.Start();
	}

	return true;
}
