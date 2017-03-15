#include "Scene.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Pathfinding.h"


#include "j1GameLayer.h"
#include <list>
#include "Player.h"

Room::Room() : Scene() {}



bool Room::Load(pugi::xml_node& config)
{
	bool ret = true;

	if (ret = (App->map->LoadRoomMap("map_testing.tmx")))
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	return ret;
}

bool Room::CleanUp()
{
	App->map->UnloadRoomMap();
	return true;
}