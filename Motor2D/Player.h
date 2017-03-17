#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Entity.h"
#include "InputManager.h"
#include "j1PerfTimer.h"


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




private:
	void Change_direction();

	//State Machine
	bool Idle();
	bool Walking(float dt);
	bool Attacking(float dt);
	bool Dodging(float dt);
	bool Damaged(float dt);

	//For Input callback
	void OnInputCallback(INPUTEVENT, EVENTSTATE);
};

#endif // !_PLAYER_H_
