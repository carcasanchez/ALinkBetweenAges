#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Entity.h"
#include "j1PerfTimer.h"

class Player : public Entity
{
	/*virtual bool Spawn(const char* file) { return true; };
	virtual bool Update(float dt) { return true; };
	virtual bool Draw();
	virtual void OnDeath() {};

	virtual bool LoadAnimations() { return true; };

public:

	// Position in Entity Manager
	std::list<Entity*>::iterator id;

	// Graphic Resources
	Sprite* sprite;
	Animation* currentAnim;
	std::map<std::pair<ACTION_STATE, DIRECTION>, Animation>	anim;

	// States
	ACTION_STATE actionState;
	DIRECTION currentDir;

	// World Position (to draw)
	iPoint lastPos;
	iPoint currentPos;

	// Collider
	Collider* col;
	iPoint colPivot;

	// Attributes
	ENTITY_TYPE type;
	int life;*/


public:

	Player();
	bool Spawn(std::string file, iPoint pos);


	//bool Awake(pugi::xml_node& config);
	bool Update(float dt);
	void Move(int x, int y);
	void UpdateCollider();

public:

	//Base
	int maxLife;
	float maxStamina;
	float stamina;
	float staminaRec;
	int speed;

	//Attack
	int attackSpeed;
	int attackTax;

	//Dodge
	int dodgeSpeed;
	int dodgeTax;
	uint64 dodgeLimit;
	iPoint dodgeDir;
	j1PerfTimer dodgeTimer;


	//SDL_Texture* playerTex;
	
	// Entity fields
	/*/PlayerAttributes* attributes = NULL;
	iPoint currentPos;
	iPoint lastPos;

	Sprite* sprite = NULL;
	---SDL_Texture* playerTex;

	std::map<std::pair<ACTION_STATE, DIRECTION>, Animation>	anim;
	Animation* currentAnim;
	ACTION_STATE actionState = IDLE;
	DIRECTION currentDir = D_DOWN;
	Collider* col;
	iPoint colPivot;*/

private:

	bool loadAnimations();

	void Change_direction();

	//State Machine
	bool Idle();
	bool Walking(float dt);
	bool Attacking(float dt);
	bool Dodging(float dt);

	


	
};

#endif // !_PLAYER_H_
