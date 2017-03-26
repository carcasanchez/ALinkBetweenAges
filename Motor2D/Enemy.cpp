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

//Makes enemy look to player. Returns true if the direction changes
bool Enemy::LookToPlayer()
{
	iPoint playerPos = (*App->game->playerId)->currentPos;

	DIRECTION prevDir = currentDir;

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

	if (prevDir == currentDir)
		return false;
	else return true;
	
}


//Move between different points
bool Enemy::Patroling(float dt)
{
	actionState = WALKING;
	if ((*App->game->playerId)->currentPos.DistanceTo(currentPos) < hostileRange)
	{
		enemyState = CHASING;
		return true;
	}

	if (patrolPoints.size() == 0)
	{
		return true;
	}
	
	iPoint dest = patrolPoints[currentPatrolPoint];

	actionState = IDLE;

	

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

//Persecute Player
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
		path.clear();
		attackTimer.Start();
		srand(time(NULL));
		flankingDir = rand() % 2;
	}

	iPoint dest = App->map->WorldToMap((*App->game->playerId)->currentPos.x, (*App->game->playerId)->currentPos.y);
	GoTo(dest, chaseSpeed, dt);

	return true;
}

//Flank player
bool Enemy::KeepDistance(float dt)
{
	LookToPlayer();


	//If link is out of range, stop chase
	if ((*App->game->playerId)->currentPos.DistanceTo(currentPos) > fightRange*1.5)
	{
		enemyState = CHASING;
	}


	//Choose randomly if the flanking direction changes
	if (abs((*App->game->playerId)->currentPos.x - currentPos.x) == abs((*App->game->playerId)->currentPos.y - currentPos.y))
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
		if (attack && currentPos.y == (*App->game->playerId)->currentPos.y || currentPos.x == (*App->game->playerId)->currentPos.x)
		{
			attackTimer.Start();
			enemyState = CHARGING;
		}
	}

	//Movement depending on the player relative direction
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

	//Change the flanking direction if the enemy hits something
	//currentPos.x += SDL_ceil(speed*dt)*movement.x;
	//currentPos.y += SDL_ceil(speed*dt)*movement.x;
	
	if (Move(SDL_ceil(speed*dt)*movement.x, SDL_ceil(speed*dt)*movement.y) == false)
	{
		flankingDir = !flankingDir;
	}


	return true;
}

//Go backwards when hit
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

//Attack player
bool Enemy::Charging(float dt)
{
	bool ret = true;

	switch (currentDir)
	{
	case D_DOWN:
			ret = Move(0, SDL_ceil(attackSpeed * dt));
			if (currentPos.y > (*App->game->playerId)->currentPos.y)
				ret = false;			
			break;

	case D_UP:
		ret = Move(0, -SDL_ceil(attackSpeed * dt));
		if (currentPos.y < (*App->game->playerId)->currentPos.y)
			ret = false;
		break;

	case D_LEFT:
		ret = Move(-SDL_ceil(attackSpeed * dt), 0);
		if (currentPos.x < (*App->game->playerId)->currentPos.x)
			ret = false;
		break;

	case D_RIGHT:
		ret = Move(SDL_ceil(attackSpeed * dt), 0);
		if (currentPos.x > (*App->game->playerId)->currentPos.x)
			ret = false;
		break;
	}
	

	if (attackTimer.ReadMs() > chargeTime)
		ret = false;

	if (ret == false)
		enemyState = CHASING;
	

	return ret;
}
