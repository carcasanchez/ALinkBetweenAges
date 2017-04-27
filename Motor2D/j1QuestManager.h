#ifndef __j1QUEST_MANAGER_H__
#define __j1QUEST_MANAGER_H__

#include "j1Module.h"
#include <list>
#include <vector>
#include "p2Point.h"
#include "j1CollisionManager.h"
#include "Enemy.h"

class Collider;
enum EVENT_TYPE
{
	//Triggers
	COLLISION_EVENT = 0,

};


enum REWARD_TYPE
{
	//Rewards
	CINEMATIC = 0,
	CREATE_ENEMY,
	LOAD_MAP
};


class Event
{
public:
	Event(EVENT_TYPE type) : type(type) {};
	~Event() {};

	EVENT_TYPE type;
};


class Reward
{
public:
	Reward(REWARD_TYPE type) : type(type) {};
	~Reward() {};

	REWARD_TYPE type;
};

class CollisionEvent : public Event
{
public:
	CollisionEvent() :Event(COLLISION_EVENT) {};

	~CollisionEvent() {};

	Collider* col;

};


class CinematicReward : public Reward
{
public:
	CinematicReward() :Reward(CINEMATIC) {};

	~CinematicReward() {};

	int cinematicId;

};


class CreateEnemyReward : public Reward
{
public:
	CreateEnemyReward() :Reward(CREATE_ENEMY) {};

	~CreateEnemyReward() {};

	ENEMY_TYPE enemy;
	iPoint mapPos;

};



class Quest
{
public:
	Quest() {};
	~Quest();

public:
	int id;
	
	Event* trigger;
	vector <Event*> steps;
	vector <Reward*> reward;
	
};

class j1QuestManager : public j1Module
{
public:

	j1QuestManager();
	~j1QuestManager();

	bool Awake(pugi::xml_node&);

	void LoadQuests(pugi::xml_node&);
	Event* createEvent(pugi::xml_node&);
	Reward* createReward(pugi::xml_node&);

	//Callbacks for each event type
	bool TriggerCollisionCallback(Collider* c);
	bool StepCollisionCallback(Collider* c);



	void RewardCallback(vector <Reward*> reward);

private:
	string path;

	list<Quest*> sleepQuests;
	list<Quest*> activeQuests;
	list<Quest*> closedQuests;

	
};

#endif