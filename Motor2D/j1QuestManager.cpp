#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1QuestManager.h"
#include "j1FileSystem.h"
#include "j1GameLayer.h"
#include "j1EntityManager.h"
#include "j1Map.h"
#include "j1CutSceneManager.h"



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
	LOG("Loading QuestManager ");
	return true;
}



void j1QuestManager::LoadQuests(pugi::xml_node & questDataFile)
{
	for (pugi::xml_node quest = questDataFile.first_child(); quest; quest = quest.next_sibling("quest"))
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
}

Event * j1QuestManager::createEvent(pugi::xml_node &it)
{
	//Event factory method
	int type = it.attribute("type").as_int();
	
	Event* ret;
	iPoint mapPos;

	switch (type)
	{		
		case COLLISION_EVENT:
			 ret = new CollisionEvent;			
			//Take collider data from XML.
			SDL_Rect rect;			
			rect.x = it.child("collider").attribute("x").as_int();
			rect.y = it.child("collider").attribute("y").as_int();
			rect.w = it.child("collider").attribute("w").as_int();
			rect.h = it.child("collider").attribute("h").as_int();	
			
			mapPos = App->map->MapToWorld(rect.x, rect.y);

			rect.x = mapPos.x;
			rect.y = mapPos.y;

			((CollisionEvent*)ret)->col = App->collisions->AddCollider(rect, COLLIDER_EVENT);
			return ret;

		case TALK_TO_EVENT:
			ret = new TalkToEvent;
			((TalkToEvent*)ret)->target = (Npc*)App->game->em->GetEntityFromId(it.child("target").attribute("id").as_int());
			return ret;
	
		case INTERRUPTOR_EVENT:
			ret = new InterruptorEvent;
			for (pugi::xml_node interr = it.child("interruptor"); interr; interr = interr.next_sibling("interruptor"))
			{
				((InterruptorEvent*)ret)->targets.push_back(App->game->em->CreateObject(1, interr.attribute("x").as_int(), interr.attribute("y").as_int(), INTERRUPTOR));
			}
			return ret;
	}

}

Reward * j1QuestManager::createReward(pugi::xml_node & it)
{
	int type = it.attribute("type").as_int();

	Reward* ret = nullptr;


	switch (type)
	{
	   case CREATE_ENEMY:
		   for (pugi::xml_node enemy = it.child("enemy"); enemy; enemy = enemy.next_sibling("enemy"))
		   {
			   ret = new CreateEnemyReward;
			   ((CreateEnemyReward*)ret)->enemy = ((ENEMY_TYPE)it.child("enemy").attribute("type").as_int());
			   ((CreateEnemyReward*)ret)->mapPos.x = it.child("enemy").attribute("x").as_int();
			   ((CreateEnemyReward*)ret)->mapPos.y = it.child("enemy").attribute("y").as_int();
			   return ret;
		   }
	   case CINEMATIC:
		   ret = new CinematicReward;
		   ((CinematicReward*)ret)->cinematicId = it.child("cinematic").attribute("id").as_int();
		   return ret;
	   case DELETE_E:
		   ret = new DeleteEntityReward;
		   ((DeleteEntityReward*)ret)->entityId = it.child("entity").attribute("id").as_int();
		   return ret;
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
								
				//Mark the collider c to delete in the next frame
				c->to_delete=true;

				if ((*it)->steps.size() == 0)
				{
					completed = (*it);
					closedQuests.push_back((*it));
					sleepQuests.erase(it);
				}
				else
				{
					activeQuests.push_back((*it));
					sleepQuests.erase(it);
				}

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
					completed = (*it);

					closedQuests.push_back((*it));
					activeQuests.erase(it);
				}

				return true;
			}
		}
	}
	return false;
}

//=========== TalkTo Callback
bool j1QuestManager::TriggerTalkToCallback(Npc * target)
{
	//Iterates all Triggers of sleep quests.
	for (std::list <Quest*>::iterator it = sleepQuests.begin(); it != sleepQuests.end(); it++)
	{


		//Check if It is a CollisionEvent
		if ((*it)->trigger->type == TALK_TO_EVENT)
		{

			if (((TalkToEvent*)(*it)->trigger)->target == target)
			{
				if ((*it)->steps.size() == 0)
				{
					completed = (*it);
					closedQuests.push_back((*it));
					sleepQuests.erase(it);
				}
				else
				{
					activeQuests.push_back((*it));
					sleepQuests.erase(it);
				}

				return true;
			}
		}

	}

	return false;
}

bool j1QuestManager::StepTalkToCallback(Npc * target)
{
	//Iterates all Steps of all active quests
	for (std::list <Quest*>::iterator it = activeQuests.begin(); it != activeQuests.end(); it++)
	{
		if ((*it)->steps.empty())
			continue;
		
		//Check if It is a CollisionEvent
		if ((*it)->steps[0]->type == TALK_TO_EVENT)
		{
			TalkToEvent* ev = ((TalkToEvent*)(*it)->steps[0]);
			if (ev->target == target)
			{
								
				//Erase the first step of the steps vector
				(*it)->steps.erase((*it)->steps.begin());
				
				//Close the quest if there's no more steps and add reward
				if ((*it)->steps.size() == 0)
				{
					completed = (*it);
					closedQuests.push_back((*it));
					activeQuests.erase(it);
				}

				return true;
			}
		}
	}
	return false;
}
//============ InterruptorCallback

bool j1QuestManager::TriggerInterruptorCallback(Object* interr)
{
	//Iterates all Triggers of sleep quests.
	for (std::list <Quest*>::iterator it = sleepQuests.begin(); it != sleepQuests.end(); it++)
	{
		//Check if It is a CollisionEvent
		if ((*it)->trigger->type == INTERRUPTOR_EVENT)
		{
			std::vector <Object*>::iterator k = ((InterruptorEvent*)(*it)->trigger)->targets.begin();
			for (; k != ((InterruptorEvent*)(*it)->trigger)->targets.end(); k++)
			{
				if ((*k) == interr)
				{
					((InterruptorEvent*)(*it)->trigger)->targets.erase(k);
					break;
				}
			}
				
				
			if (((InterruptorEvent*)(*it)->trigger)->targets.empty())
			{
				if ((*it)->steps.size() == 0)
				{
					completed = (*it);
					closedQuests.push_back((*it));
					sleepQuests.erase(it);
				}
				else
				{
					activeQuests.push_back((*it));
					sleepQuests.erase(it);
				}

				return true;
			}
		}

	}

	return false;
}

bool j1QuestManager::StepInterruptorCallback(Object* interr)
{
	//Iterates all Steps of all active quests
	for (std::list <Quest*>::iterator it = activeQuests.begin(); it != activeQuests.end(); it++)
	{
		if ((*it)->steps.empty())
			continue;

		if ((*it)->steps[0]->type == INTERRUPTOR_EVENT)
		{
			std::vector <Object*>::iterator k = ((InterruptorEvent*)(*it)->steps[0])->targets.begin();
			for (; k != ((InterruptorEvent*)(*it)->steps[0])->targets.end(); k++)
			{
				if ((*k) == interr)
				{
					((InterruptorEvent*)(*it)->steps[0])->targets.erase(k);
					break;
				}
			}


			if (((InterruptorEvent*)(*it)->steps[0])->targets.empty())
			{
				//Erase the first step of the steps vector
				(*it)->steps.erase((*it)->steps.begin());

				//Close the quest if there's no more steps and add reward
				if ((*it)->steps.size() == 0)
				{
					completed = (*it);
					closedQuests.push_back((*it));
					activeQuests.erase(it);
				}
				return true;
			}
		}
	}
	return false;
}


//====================


void j1QuestManager::RewardCallback(vector <Reward*> reward)
{
	iPoint pos = {0, 0};
	ENEMY_TYPE type;

	for (std::vector <Reward*>::iterator it = reward.begin(); it != reward.end(); it++)
	{
		
		switch ((*it)->type)
		{			
		case CREATE_ENEMY:
			pos = ((CreateEnemyReward*)*it)->mapPos;
			type = ((CreateEnemyReward*)*it)->enemy;
			
			App->game->em->CreateEnemy(1, type, pos.x, pos.y, vector<iPoint>());
			break;

		case CINEMATIC:
			pos = { 0, 0 };
			App->cutsceneM->StartCutscene(((CinematicReward*)*it)->cinematicId);
			break;
		case DELETE_E:
			App->game->em->GetEntityFromId(((DeleteEntityReward*)*it)->entityId)->life = -1;
			break;
		}
	}
}


bool j1QuestManager::Update(float dt)
{
	if (completed)
	{
		RewardCallback(completed->reward);
		completed = nullptr;
	}

	return true;
}


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




