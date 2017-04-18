#include "DarkZelda.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "Animation.h"
#include "j1EntityManager.h"
#include "Object.h"


bool DarkZelda::Spawn(std::string file, iPoint pos)
{
	bool ret = true;

	//set position
	currentPos = lastPos = pos;

	// load xml attributes
	pugi::xml_document	attributesFile;
	char* buff;
	int size = App->fs->Load(file.c_str(), &buff);
	pugi::xml_parse_result result = attributesFile.load_buffer(buff, size);
	RELEASE_ARRAY(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}

	else
	{
		pugi::xml_node attributes = attributesFile.child("attributes").child("dark_zelda");

		LoadAttributes(attributes);

		enemyState = PATROLING;
		actionState = WALKING;

		hostileRange = attributes.child("ranges").attribute("hostile").as_int(0);
		fightRange = attributes.child("ranges").attribute("fight").as_int(0);
		attackRatio = attributes.child("ratios").attribute("attack").as_int(0);
		outFightRange = attributes.child("ranges").attribute("out").as_int(0);
		chargeTime = attributes.child("ratios").attribute("charge_time").as_int(0);

		chaseSpeed = attributes.child("combat_speeds").attribute("chase_speed").as_int(0);
		flankingSpeed = attributes.child("combat_speeds").attribute("flanking_speed").as_int(0);
		attackSpeed = attributes.child("combat_speeds").attribute("attack_speed").as_int(0);

	}
	return ret;
}




bool DarkZelda::Update(float dt)
{
	switch (phase)
	{	
	case 1:
		switch (enemyState)
		{

		}
		break;
	case 2:
		switch (enemyState)
		{

		}
		break;
	case 3:
		switch (enemyState)
		{

		}
		break;
	}
	
	return false;
}