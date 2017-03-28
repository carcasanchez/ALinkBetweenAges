#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Entity.h"
#include "NPC.h"
#include "InputManager.h"
#include "j1PerfTimer.h"

enum PLAYER_STATE
{
	ACTIVE=0,
	EVENT
};

class Player : public Entity, public InputListener
{
public:

	Player();
	bool Spawn(std::string file, iPoint pos);
	bool Update(float dt);
	void OnDeath();

public:

	//Base
	int maxLife;
	float maxStamina;
	float stamina;
	float staminaRec;


	//Attack
	int attackSpeed;
	int attackTax;

	//Dodge
	int dodgeSpeed;
	int dodgeTax;
	uint64 dodgeLimit;
	iPoint dodgeDir;
	j1PerfTimer dodgeTimer;


	bool invulnerable;
	Npc* toTalk = nullptr;

	PLAYER_STATE playerState = ACTIVE;

private:
	void Change_direction();
	void ManageStamina(float dt);

	//State Machine
	bool Idle();
	bool Walking(float dt);
	bool Attacking(float dt);
	bool Dodging(float dt);
	bool Damaged(float dt);

	bool Talking(float dt);

	bool dodging = false;
	//For Input callback
	void OnInputCallback(INPUTEVENT, EVENTSTATE);

	// For attack
	Collider* swordCollider;
	iPoint swordColliderPivot = { 0, 0 };

	//Damaged
	int hitTime;
	int damagedTime;
	int damagedSpeed;

	//Create a collider for Link's sword taking in account his direction
	void createSwordCollider();
	void updateSwordCollider();
	void resetSwordCollider();
};

#endif // !_PLAYER_H_
