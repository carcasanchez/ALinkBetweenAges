#ifndef _NPC_H_
#define _NPC_H_

#include "Entity.h"
#include "InputManager.h"
#include "j1PerfTimer.h"

enum NPC_TYPE
{
	NPC_1 = 0 
};


class Npc : public Entity, public InputListener
{
public:

	Npc();

	NPC_TYPE npcType;
	bool Spawn(std::string file, iPoint pos, NPC_TYPE);
	virtual bool Update(float dt);
	virtual void OnDeath();

public:

	
private:
	//void Change_direction();

	//State Machine
	//bool Idle();
	//bool Walking(float dt);

};


class Npc1 : public Npc
{
public:
	Npc1() : Npc() { npcType = NPC_1; };
};
#endif // !_PLAYER_H_