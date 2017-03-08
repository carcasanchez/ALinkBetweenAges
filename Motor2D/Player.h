#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Animation.h"
#include "j1CollisionManager.h"
#include <map>
#include "PugiXml\src\pugixml.hpp"
#include "j1PerfTimer.h"

class Sprite;

enum ACTION_STATE
{
	IDLE = 0,
	WALKING,
	ATTACKING,
	DODGING
};

enum DIRECTION
{
	D_UP,
	D_DOWN,
	D_RIGHT,
	D_LEFT
};

class Player
{
public:

	Player();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	iPoint GetWorldPosition();
	void Move(int x, int y);

	int speed;
	int attacking_speed;
	Collider* col;
	iPoint collider_pivot;

private:

	bool loadAnimations();

	void Change_direction();

	//State Machine
	bool Idle();
	bool Walking(float dt);
	bool Attacking(float dt);
	bool Dodging(float dt);

	void UpdateCollider();



private:

	//PlayerAttributes* attributes = NULL;
	iPoint worldPosition;	// position in pixels of player // This is the point of the pivot in the world //The feet of the player
	iPoint mapPosition;		// position of the tile where player is

	Sprite* sprite = NULL;
	SDL_Texture* playerTex;

	std::map<std::pair<ACTION_STATE, DIRECTION>, Animation>	playerAnim;
	Animation* current_animation;
	ACTION_STATE player_state = IDLE;
	DIRECTION current_direction = D_DOWN;
	
	//Basic stats
	float max_stamina;
	float stamina;
	float stamina_recover_val;
	int life;

	//Attack data
	int stamina_atk_tax;


	//Dodge data
	int stamina_dodge_tax;
	iPoint dodge_direction;
	j1PerfTimer dodge_timer;
	uint64 dodge_limit;
	int dodge_speed;

	
};

#endif // !_PLAYER_H_
