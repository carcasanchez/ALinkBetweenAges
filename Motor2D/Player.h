#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Entity.h"
#include "NPC.h"
#include "Bezier.h"
#include "InputManager.h"
#include "j1PerfTimer.h"
#include "Object.h"




enum PLAYER_STATE
{
	ACTIVE=0,
	EVENT,
	CUTSCENE
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

	void GetHit(Entity* agressor);

	void ChangeAge(LINK_AGE);

public:

	bool stopX, stopY = false;

	PLAYER_STATE playerState = ACTIVE;
	LINK_AGE age = YOUNG;

	//Base
	int maxLife;
	int bonusLife = 0;
	float maxStamina;
	float stamina;
	float staminaRec;
	int rupees = 0 ;

	//Attack
	int attackSpeed;
	int attackTax;

	//Dodge
	int dodgeSpeed;
	int dodgeTax;
	int spinTax;
	uint64 dodgeLimit;
	iPoint dodgeDir;
	j1PerfTimer dodgeTimer;


	bool invulnerable;
	bool dodging = false;
	int changeAge = -1;
	Npc* toTalk = nullptr;

	iPoint toJump = { 0, 0 };
	iPoint jumpOrigin;

	bool sceneOverride = false;

	Object* pickedObject = nullptr;
	j1PerfTimer showObjectTimer;


	int maxArrows;
	int maxBombs;
	int maxLifePotions;
	int maxStaminaPotions;
	int arrows = 0;
	int bombs = 0;
	int lifePotions = 0;
	int staminaPotions = 0;

	bool ableToSpin = false;

	std::list <OBJECT_TYPE> inventory;
	OBJECT_TYPE				holded_item = NO_OBJECT;
	int equippedObject = 0;



	vector <int> completedQuests;

	bool bossKeyEquipped = false;


private:
	void Change_direction();
	void ManageStamina(float dt);

	//State Machine
	bool Idle();
	bool Walking(float dt);
	bool Attacking(float dt);
	bool Dodging(float dt);
	bool Damaged(float dt);
	bool ShootingBow(float dt);
	bool Spinning(float dt);
	bool Jumping(float dt);

	bool Talking(float dt);




	void ShowPickedObject();
	void UseObject(float dt = 0);

	
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


	//For jumping
	CBeizier jumpCurve;
	j1PerfTimer jumpTimer;
	
	float forceUp = 0, forceDown = 0;

	//For stamina and potions
	bool only_one_time = false;

};

#endif // !_PLAYER_H_
