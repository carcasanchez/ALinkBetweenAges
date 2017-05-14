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
#include "Player.h"
#include "Object.h"
#include "j1Audio.h"


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
		
		attackRatio = attributes.child("ratios").attribute("attack").as_int(0);
		outFightRange = attributes.child("ranges").attribute("out").as_int(0);
		hostileRange = attributes.child("ranges").attribute("hostile").as_int(0);

	}
	return ret;
}

bool Octorok::Update(float dt)
{
	
	switch (enemyState)
	{
	case (PATROLING):
		Patroling(dt);
		break;
	case (THROWING_ATTACK):
		ThrowingAttack(dt);
		break;
	case(STEP_BACK):
		StepBack(dt);
		break;
	}

	return false;
}



bool Octorok::Patroling(float dt)
{
	actionState = WALKING;
	if (App->game->em->player->currentPos.DistanceTo(currentPos) < hostileRange)
	{
		enemyState = THROWING_ATTACK;
		attackTimer.Start();
		return true;
	}

	if (patrolPoints.size() == 0)
	{
		actionState = IDLE;
		return true;
	}

	if (GoTo(patrolPoints[currentPatrolPoint], speed, dt) == false)
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

bool Octorok::ThrowingAttack(float dt)
{
	actionState = IDLE;
	LookToPlayer();

	if (attackTimer.ReadMs() > attackRatio)
	{
		attackTimer.Start();
		Object* rock = App->game->em->ActiveObject(currentPos.x, currentPos.y, OCTO_STONE);
		rock->currentDir = currentDir;
		App->audio->PlayFx(28);
	}

	if (App->game->em->player->currentPos.DistanceTo(currentPos) > outFightRange)
	{
		enemyState = PATROLING;
	}

	return true;
}
