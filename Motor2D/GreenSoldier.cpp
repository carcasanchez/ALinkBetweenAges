#include "j1App.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "GreenSoldier.h"


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


		hostileRange = attributes.child("hostile").attribute("range").as_int();
		

		//TODO: LOAD THIS FROM XML
		patrolPoints.push_back({ 1, 1 });
		patrolPoints.push_back({ 1, 10 });
		patrolPoints.push_back({ 20, 10 });
		patrolPoints.push_back({ 40, 10 });
	}
	return ret;
}

bool GreenSoldier::Update(float dt)
{

	switch (enemyState)
	{
	case(PATROLING):
		Patroling(dt);
		break;
	case(CHASING):
		Chasing(dt);
		break;

	}
	
	return false;
}


