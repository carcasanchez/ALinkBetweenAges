#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1QuestManager.h"
#include "j1FileSystem.h"
#include "j1GameLayer.h"
#include "j1EntityManager.h"
#include "j1Map.h"


j1QuestManager::j1QuestManager() : j1Module()
{
	name = ("quest");
}

j1QuestManager::~j1QuestManager()
{
	
	for (std::list <Quest*>::iterator it = sleepQuests.begin(); it != sleepQuests.end(); it++)
		sleepQuests.erase(it);

	for (std::list <Quest*>::iterator it = activeQuests.begin(); it != activeQuests.end(); it++)
		activeQuests.erase(it);
	
	for (std::list <Quest*>::iterator it = closedQuests.begin(); it != closedQuests.end(); it++)
		closedQuests.erase(it);
}

bool j1QuestManager::Awake(pugi::xml_node& config)
{
	//Load the path of QuestData file from Config
	LOG("Loading QuestManager data");
	path = config.child("data").attribute("file").as_string();
	return true;
}

bool j1QuestManager::Start()
{
	bool ret = true;


	//Load QuestData File
	pugi::xml_document	questDataFile;
	char* buff;
	int size = App->fs->Load(path.c_str(), &buff);
	pugi::xml_parse_result result = questDataFile.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load questData xml file. Pugi error: %s", result.description());
		ret = false;
	}


	else for (pugi::xml_node quest = questDataFile.child("quests").first_child(); quest; quest = quest.next_sibling("quest"))
	{
		//Load quest data from XML
		Quest* new_quest = new Quest();
		new_quest->id = quest.attribute("id").as_int();

		pugi::xml_node reward;
		for (reward = quest.child("reward"); reward; reward = reward.next_sibling("reward"))
		{
			new_quest->reward.push_back(createReward(reward));
		}
		

		new_quest->trigger = createEvent(quest.child("trigger"));

		pugi::xml_node step;
		for (step = quest.child("step"); step; step = step.next_sibling("step"))
		{
			new_quest->steps.push_back(createEvent(step));
		}

		sleepQuests.push_back(new_quest);	
	}
	


	return ret;
}

Event * j1QuestManager::createEvent(pugi::xml_node &it)
{
	//Event factory method
	int type = it.attribute("type").as_int();
	
	Event* ret;

	switch (type)
	{		
		case (COLLISION_EVENT):
			 ret = new CollisionEvent();			
			//Take collider data from XML.
			SDL_Rect rect;			
			rect.x = it.child("collider").attribute("x").as_int();
			rect.y = it.child("collider").attribute("y").as_int();
			rect.w = it.child("collider").attribute("w").as_int();
			rect.h = it.child("collider").attribute("h").as_int();	
			
			iPoint mapPos = App->map->MapToWorld(rect.x, rect.y);

			rect.x = mapPos.x;
			rect.y = mapPos.y;

			((CollisionEvent*)ret)->col = App->collisions->AddCollider(rect, COLLIDER_EVENT);
			return ret;
	}

}

Reward * j1QuestManager::createReward(pugi::xml_node & it)
{
	int type = it.attribute("type").as_int();

	Reward* ret = nullptr;


	switch (type)
	{
	   case(CREATE_ENEMY):
		   for (pugi::xml_node enemy = it.child("enemy"); enemy; enemy = enemy.next_sibling("enemy"))
		   {
			   ret = new CreateEnemyReward();
			   ((CreateEnemyReward*)ret)->enemy = ((ENEMY_TYPE)it.child("enemy").attribute("type").as_int());
			   ((CreateEnemyReward*)ret)->mapPos.x = it.child("enemy").attribute("x").as_int();
			   ((CreateEnemyReward*)ret)->mapPos.y = it.child("enemy").attribute("y").as_int();
			   return ret;
		   }
		
	}


	return ret;
}

//=========== Collision Callbacks
bool j1QuestManager::TriggerCollisionCallback(Collider* c)
{
	//Iterates all Triggers of sleep quests.
	for (std::list <Quest*>::iterator it = sleepQuests.begin(); it != sleepQuests.end(); it++)
	{

		
		//Check if It is a CollisionEvent
		if ((*it)->trigger->type == COLLISION_EVENT)
		{

			if (((CollisionEvent*)(*it)->trigger)->col == c)
			{
				activeQuests.push_back((*it));
				sleepQuests.erase(it);

				
				//Mark the collider c to delete in the next frame
				c->to_delete=true;
				return true;
			}
		}
		
	}

	return false;
}

bool j1QuestManager::StepCollisionCallback(Collider * c)
{
	//Iterates all Steps of all active quests
	for (std::list <Quest*>::iterator it = activeQuests.begin(); it != activeQuests.end(); it++)
	{
		
		//Check if It is a CollisionEvent
		if ((*it)->steps[0]->type == COLLISION_EVENT)
		{
			CollisionEvent* ev = ((CollisionEvent*)(*it)->steps[0]);
			if (ev->col == c)
			{
								
				//Erase the first step of the steps vector
				(*it)->steps.erase((*it)->steps.begin());

				//Mark the collider c to delete in the next frame
				c->to_delete = true;

				//Close the quest if there's no more steps and add reward
				if ((*it)->steps.size() == 0)
				{
					RewardCallback((*it)->reward);

					closedQuests.push_back((*it));
					activeQuests.erase(it);
				}

				return true;
			}
		}
	}
	return false;
}


void j1QuestManager::RewardCallback(vector <Reward*> reward)
{
	for (std::vector <Reward*>::iterator it = reward.begin(); it != reward.end(); it++)
	{
		switch ((*it)->type)
		{
		case CREATE_ENEMY:
			iPoint pos;
			pos = ((CreateEnemyReward*)*it)->mapPos;
			App->game->em->CreateEnemy(1,((CreateEnemyReward*)*it)->enemy, pos.x, pos.y, vector<iPoint>());
			break;
		}

	}
	


}


//BONUS: create more Trigger and Step callbacks for each type of Event. Search for the correct place to call them.

//=============================================

Quest::~Quest()
{
	//Destroy each event of each quest
	delete trigger;


	for (vector <Reward*>::iterator it = reward.begin(); it != reward.end(); it++)
	{
		delete (*it);
		reward.erase(it);
	}


	for (vector <Event*>::iterator it = steps.begin(); it != steps.end(); it++)
	{
		delete (*it);
		steps.erase(it);
	}
	
}




