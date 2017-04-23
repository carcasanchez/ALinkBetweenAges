#include "j1App.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "Animation.h"
#include "Wizdrove.h"
#include "j1EntityManager.h"
#include "Player.h"
#include "Object.h"
#include "j1CollisionManager.h"


bool Wizdrove::Spawn(std::string file, iPoint pos)
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
		pugi::xml_node attributes = attributesFile.child("attributes").child("wizdrove");

		LoadAttributes(attributes);

		enemyState = PATROLING;
		actionState = IDLE;

		attackRatio = 1000;
		outFightRange = 100;
		hostileRange = 100;

	}
	return ret;
}

bool Wizdrove::Update(float dt)
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
	case (PREPARING_ATTACK):
		PreparingAttack(dt);
		break;
	}

	return false;
}



bool Wizdrove::Patroling(float dt)
{
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

bool Wizdrove::ThrowingAttack(float dt)
{
	actionState = ATTACKING;
	LookToPlayer();


	if (currentAnim->isOver())
	{
		attackTimer.Start();
		enemyState = PREPARING_ATTACK;
		currentAnim->Reset();
		iPoint mapPos = App->map->WorldToMap(currentPos.x, currentPos.y);
		Object* slash = App->game->em->CreateObject(1, mapPos.x, mapPos.y, MAGIC_SLASH);
		slash->currentDir = currentDir;
	
		if (currentDir == D_RIGHT || currentDir == D_LEFT)
		{
			slash->col->rect.w = 8;
			slash->col->rect.h = 16;
			slash->colPivot = { 3, 8 };
		}

		slash->currentPos = currentPos;
		slash->currentPos.y -= 5;
	
	}


	return true;
}

bool Wizdrove::PreparingAttack(float dt)
{
	LookToPlayer();
	actionState = IDLE;

	if (attackTimer.ReadMs() > attackRatio)
	{
		enemyState = THROWING_ATTACK;
	}

	if (App->game->em->player->currentPos.DistanceTo(currentPos) > outFightRange)
	{
		enemyState = PATROLING;
	}

	return true;
}
