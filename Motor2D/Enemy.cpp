#include "j1App.h"
#include "Enemy.h"
#include "j1GameLayer.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1CollisionManager.h"
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
	if ((*App->game->playerId)->currentPos.DistanceTo(currentPos) >= hostileRange)
	{
		enemyState = PATROLING;
		return true;
	}

	iPoint dest = App->map->WorldToMap((*App->game->playerId)->currentPos.x, (*App->game->playerId)->currentPos.y);
	GoTo(dest, speed*2, dt);

	return true;
}
