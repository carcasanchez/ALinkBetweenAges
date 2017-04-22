#include "j1App.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "Animation.h"
#include "Octorok.h"
#include "j1EntityManager.h"
#include "Object.h"


bool Octorok::Spawn(std::string file, iPoint pos)
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
		pugi::xml_node attributes = attributesFile.child("attributes").child("octorok");

		LoadAttributes(attributes);

		enemyState = PATROLING;
		actionState = IDLE;


	}
	return ret;
}

bool Octorok::Update(float dt)
{
	LookToPlayer();

	switch (enemyState)
	{
	case(STEP_BACK):
		StepBack(dt);
		break;
	}

	return false;
}



