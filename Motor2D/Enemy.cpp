#include "j1App.h"
#include "Enemy.h"
#include "j1GameLayer.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include <math.h>

bool Enemy::Update(float dt)
{
	SearchForPlayer(speed, dt);

	return true;
}

bool Enemy::SearchForPlayer(int speed, float dt)
{
	//TODO clean this
	iPoint player_tile = App->map->WorldToMap(App->game->playerId._Mynode()->_Myval->currentPos.x, App->game->playerId._Mynode()->_Myval->currentPos.y);
	iPoint immediateDest;

	//Create path if player changes tile
	if (player_tile != currentDest)
	{
		currentDest = player_tile;
		iPoint origin = App->map->WorldToMap(currentPos.x, currentPos.y);
		if (App->pathfinding->CreatePath(origin, currentDest))
		{
			path = App->pathfinding->ReturnPath();
			path.erase(path.begin());
			//for (int i = 0, j = path.size(); i < j; i++)
				//LOG("PATH %i: x-%i, y-%i", i, path[i].x, path[i].y);
			//LOG("----------------------------------------");
		}
	}


	if (path.size() != 0)
	{
		
		immediateDest = App->map->GetTileCenter(path[0]);

		if (immediateDest.x > currentPos.x)
			currentPos.x += SDL_ceil(speed * dt);
		else if (immediateDest.x < currentPos.x)
			currentPos.x -= SDL_ceil(speed * dt);

		if (immediateDest.y > currentPos.y)
			currentPos.y += SDL_ceil(speed * dt);
		else if (immediateDest.y < currentPos.y)
			currentPos.y -= SDL_ceil(speed * dt);

		//TODO: change iPoint pos to fPoint pos
		if (abs(immediateDest.x-currentPos.x) < 2 && abs(immediateDest.y - currentPos.y) < 2)
		{
			path.erase(path.begin());
		}
	}
	
	





	return false;
}
