#include "j1App.h"
#include "Enemy.h"
#include "j1GameLayer.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1CollisionManager.h"
#include "j1Render.h"
#include "j1EntityManager.h"
#include "Player.h"
#include <math.h>
#include <time.h>
#include "Player.h"
#include "Animation.h"
#include "HUD.h"



void Enemy::OnDeath()
{
	toDelete = true;
}

//Move between different points
bool Enemy::Patroling(float dt)
{
	actionState = WALKING;
	if (App->game->em->player->currentPos.DistanceTo(currentPos) < hostileRange)
	{
		enemyState = CHASING;
		return true;
	}

	if (patrolPoints.size() == 0)
	{
		actionState = IDLE;
		return true;
	}
	
	if (GoTo(patrolPoints[currentPatrolPoint], speed, dt) == false)
	{
		currentPatrolPoint++;
		if (currentPatrolPoint == patrolPoints.size())
		{
			patrolPoints._Swap_all(patrolPoints);
			currentPatrolPoint = 0;
		}
	};

	return true;
}

//Persecute Player
bool Enemy::Chasing(float dt)
{
	int playerDistance = App->game->em->player->currentPos.DistanceTo(currentPos);
	if (playerDistance >= hostileRange)
	{
		enemyState = PATROLING;
		return true;
	}
	else if (playerDistance <= fightRange)
	{
		enemyState = KEEP_DISTANCE;
		path.clear();
		attackTimer.Start();
		srand(time(NULL));
		flankingDir = rand() % 2;
	}
  
	iPoint playerTile = App->map->WorldToMap(App->game->em->player->currentPos.x, App->game->em->player->currentPos.y);

	GoTo(playerTile, chaseSpeed, dt);
	LookToPlayer();

	return true;
}

//Flank player
bool Enemy::KeepDistance(float dt)
{
	LookToPlayer();


	//If link is out of range, stop chase
	if (App->game->em->player->currentPos.DistanceTo(currentPos) > fightRange*1.5)
	{
		enemyState = CHASING;
	}


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
	if (attackTimer.ReadMs() > attackRatio)
	{	
		srand(time(NULL));
		bool attack = rand() % 2;
		if (abs(currentPos.y - App->game->em->player->currentPos.y)<3 || abs(currentPos.x - App->game->em->player->currentPos.x)<3)
		{
			attackTimer.Start();
			enemyState = CHARGING;
		}
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
	if (Move(SDL_ceil(flankingSpeed*dt)*flankingMovement.x, SDL_ceil(flankingSpeed*dt)*flankingMovement.y) == false)
	{
		flankingDir =!flankingDir;
	}


	

	return true;
}

//Go backwards when hit
bool Enemy::StepBack(float dt)
{
	iPoint movement;

	if (App->game->em->player->currentPos.x > currentPos.x)
		movement.x = -1;
	else movement.x = +1;
	if (App->game->em->player->currentPos.y > currentPos.y)
		movement.y = -1;
	else movement.y = +1;

	Move(SDL_ceil(movement.x*speed*dt*5), SDL_ceil(movement.y*speed*dt*5));
	
	if (damagedTimer.ReadMs() > 100 && pushedBackTimer.ReadMs() > 200)
	{
		enemyState = CHASING;
		sprite->tint = { 255, 255, 255, 255 };
		if (life == 0)
		{
			life--;
		}
	}

	return true;
}

//Attack player
bool Enemy::Charging(float dt)
{
	bool ret = true;

	switch (currentDir)
	{
	case D_DOWN:
			ret = Move(0, SDL_ceil(attackSpeed * dt));			
			break;

	case D_UP:
		ret = Move(0, -SDL_ceil(attackSpeed * dt));
		break;

	case D_LEFT:
		ret = Move(-SDL_ceil(attackSpeed * dt), 0);
		break;

	case D_RIGHT:
		ret = Move(SDL_ceil(attackSpeed * dt), 0);
		break;
	}
	

	if (attackTimer.ReadMs() > chargeTime)
		ret = false;

	if (ret == false)
	{
		enemyState = KEEP_DISTANCE;
		attackTimer.Start();
	}
	

	return ret;
}
