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

enum LINK_AGE
{
	YOUNG=0,
	MEDIUM,
	ADULT
};

class Player : public Entity, public InputListener
{
public:

	Player();
	bool Spawn(std::string file, iPoint pos);
	bool Update(float dt);
	void OnDeath();

	void ChangeAge(LINK_AGE);

	//THRASH VL
	bool win_con = false;
	bool attack_vicente = false;
	int defeatedEnemies = 0;

public:


	PLAYER_STATE playerState = ACTIVE;
	LINK_AGE age = YOUNG;

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
	bool dodging = false;
	int changeAge = -1;
	Npc* toTalk = nullptr;

	bool sceneOverride = false;

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


	//For Input callback
	void OnInputCallback(INPUTEVENT, EVENTSTATE);

	// For attack
	Collider* swordCollider;
	iPoint swordColliderPivot = { 0, 0 };
	j1PerfTimer stickTimer;

	//Damaged
	int hitTime;
	int damagedTime;
	int damagedSpeed;

	//Create a collider for Link's sword taking in account his direction
	void createSwordCollider();
	void updateSwordCollider();
	void resetSwordCollider();

	//Dialogue
	void NextDialog();
	bool firstText = true; //Enter once in to the function
};

#endif // !_PLAYER_H_
