#include "j1App.h"
#include "Enemy.h"
#include "j1GameLayer.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1CollisionManager.h"
#include "j1Render.h"
#include <math.h>

bool Enemy::Update(float dt)
{
	return true;
}



void Enemy::OnDeath()
{
	toDelete = true;
	col->to_delete = true;
}

void Enemy::Repeal()
{
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
	}

	iPoint dest = App->map->WorldToMap((*App->game->playerId)->currentPos.x, (*App->game->playerId)->currentPos.y);
	GoTo(dest, speed*2, dt);

	return true;
}

bool Enemy::KeepDistance(float dt)
{
	if ((*App->game->playerId)->currentPos.DistanceTo(currentPos) > fightRange)
	{
		enemyState = CHASING;
	}
	return true;
}

bool Enemy::StepBack(float dt)
{
	iPoint movement;

	if ((*App->game->playerId)->currentPos.x > currentPos.x)
		movement.x -= 1;
	else movement.x += 1;
	if ((*App->game->playerId)->currentPos.y > currentPos.y)
		movement.y -= 1;
	else movement.y += 1;

	Move(SDL_ceil(movement.x*speed*dt*5), SDL_ceil(movement.y*speed*dt*5));

	if ((*App->game->playerId)->currentPos.DistanceTo(currentPos) > fightRange*2)
	{
		enemyState = CHASING;
		sprite->tint = { 255, 255, 255, 255 };
	}

	return true;
}
