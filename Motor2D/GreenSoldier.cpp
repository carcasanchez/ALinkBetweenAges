#include "j1App.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "GreenSoldier.h"
#include "j1Input.h"
#include  "j1Map.h"


bool GreenSoldier::Spawn(std::string file, iPoint pos)
{
	bool ret = true;

	//set position
	currentPos = lastPos = pos;

	// load xml attributes
	pugi::xml_document	attributesFile;
	char* buff;
	int size = App->fs->Load(file.c_str(), &buff);
	pugi::xml_parse_result result = attributesFile.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}

	else
	{
		pugi::xml_node attributes = attributesFile.child("attributes").child("green_soldier");

		LoadAttributes(attributes);
	}
	return ret;
}

bool GreenSoldier::Update(float dt)
{

		iPoint origin = App->map->WorldToMap(currentPos.x, currentPos.y);
		iPoint dest = App->map->WorldToMap(App->game->playerId._Mynode()->_Myval->currentPos.x, App->game->playerId._Mynode()->_Myval->currentPos.y);
		if (App->pathfinding->CreatePath(origin,dest))
		{
			path = App->pathfinding->ReturnPath();
		}
	

	if (path.size() != 0)
	{
		if (App->map->WorldToMap(currentPos.x, currentPos.y) == path[0])
		{
			path.erase(path.begin());
		}
		iPoint movement = path[0] - App->map->WorldToMap(currentPos.x, currentPos.y);
		currentPos+= movement;
	}

	return true;
}

