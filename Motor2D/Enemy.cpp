#include "j1App.h"
#include "Enemy.h"
#include "j1GameLayer.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1CollisionManager.h"
#include "j1Render.h"
#include <math.h>
#include <time.h>

bool Enemy::Update(float dt)
{
	return true;
}



void Enemy::OnDeath()
{
	toDelete = true;
	col->to_delete = true;
}

void Enemy::LookToPlayer()
{
	iPoint playerPos = (*App->game->playerId)->currentPos;

	if (abs(playerPos.x - currentPos.x) < abs(playerPos.y - currentPos.y))
	{
		if (playerPos.y > currentPos.y)
			currentDir = D_DOWN;
		else currentDir = D_UP;
		
		
	}
	else
	{
		if (playerPos.x < currentPos.x)
			currentDir = D_LEFT;
		else currentDir = D_RIGHT;
	}
	
}



bool Enemy::Patroling(float dt)
{
	iPoint dest = patrolPoints[currentPatrolPoint];

	actionState = IDLE;

	if ((*App->game->playerId)->currentPos.DistanceTo(currentPos) < hostileRange)
	{
		enemyState = CHASING;
		return true;
	}

	if (GoTo(dest, speed, dt) == false)
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

bool Enemy::Chasing(float dt)
{
	int playerDistance = (*App->game->playerId)->currentPos.DistanceTo(currentPos);
	if (playerDistance >= hostileRange)
	{
		enemyState = PATROLING;
		return true;
	}
	else if (playerDistance <= fightRange)
	{
		enemyState = KEEP_DISTANCE;
		fightTimer.Start();
		srand(time(NULL));
		fightDir = rand() % 2;
	}

	iPoint dest = App->map->WorldToMap((*App->game->playerId)->currentPos.x, (*App->game->playerId)->currentPos.y);
	GoTo(dest, speed*2, dt);

	return true;
}

bool Enemy::KeepDistance(float dt)
{
	if ((*App->game->playerId)->currentPos.DistanceTo(currentPos) > fightRange*1.5)
	{
		enemyState = CHASING;
		
	}

	LookToPlayer();

	if (fightTimer.ReadMs() > 500)
	{
		srand(time(NULL));
		bool change = rand() % 2;
		if(change)
		{ 			
			fightTimer.Start();
			fightDir = !fightDir;
		}
	}

	if (fightDir)
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

	if (Move(SDL_ceil(speed*dt)*movement.x, SDL_ceil(speed*dt)*movement.y) == false)
	{
		fightDir = !fightDir;
		fightTimer.Start();
	}


	return true;
}

bool Enemy::StepBack(float dt)
{
	iPoint movement;

	if ((*App->game->playerId)->currentPos.x > currentPos.x)
		movement.x = -1;
	else movement.x = +1;
	if ((*App->game->playerId)->currentPos.y > currentPos.y)
		movement.y = -1;
	else movement.y = +1;

	Move(SDL_ceil(movement.x*speed*dt*5), SDL_ceil(movement.y*speed*dt*5));

	if (damagedTimer.ReadMs() > 100)
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
