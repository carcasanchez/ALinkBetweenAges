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
	~Npc() {};

	NPC_TYPE npcType;
	bool Spawn(std::string file, iPoint pos, NPC_TYPE);
	virtual bool Update(float dt);

public:

	int npcId;
	int dialogState;

	
private:
	//void Change_direction();

	//State Machine


};


class Npc1 : public Npc
{
public:
	Npc1() : Npc() { npcType = NPC_1; };
};
#endif // !_PLAYER_H_