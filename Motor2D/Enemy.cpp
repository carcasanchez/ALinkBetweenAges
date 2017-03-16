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

	if (abs(currentPos.x - App->game->playerId._Mynode()->_Myval->currentPos.x) <= App->map->data->tile_width
		&& abs(currentPos.y - App->game->playerId._Mynode()->_Myval->currentPos.y) <= App->map->data->tile_height)
	{
		immediateDest = App->game->playerId._Mynode()->_Myval->currentPos;
	}
	else
	{
		if (player_tile != currentDest)
		{
			currentDest = player_tile;
			iPoint origin = App->map->WorldToMap(currentPos.x, currentPos.y);
			if (App->pathfinding->CreatePath(origin, currentDest))
			{
				path = App->pathfinding->ReturnPath();
				path.erase(path.begin());
			}
		}


		if (path.size() != 0)
		{
			//LOG("IMMEDIATE DEST: x %i   y %i", immediateDest.x, immediateDest.y);
			immediateDest = App->map->GetTileCenter(path[0]);
			if (currentPos == immediateDest)
			{
				path.erase(path.begin());
				immediateDest = App->map->GetTileCenter(path[0]);
			}			
		}
	}
	


	if (immediateDest.x > currentPos.x)
		currentPos.x += SDL_ceil(speed * dt);
	else if (immediateDest.x < currentPos.x)
		currentPos.x -= SDL_ceil(speed * dt);

	if (immediateDest.y > currentPos.y)
		currentPos.y += SDL_ceil(speed * dt);
	else if (immediateDest.y < currentPos.y)
		currentPos.y -= SDL_ceil(speed * dt);



	return false;
}
