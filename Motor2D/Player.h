#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Animation.h"
#include <map>
#include "PugiXml\src\pugixml.hpp"

class Sprite;

enum ACTION_STATE
{
	IDLE = 0,
	WALKING,
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
	bool UpdateTicks();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	//Check different player status
	

private:

	bool loadAnimations();

	void Change_direction();

	//State Machine
	bool Idle();
	bool Walking();


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
};

#endif // !_PLAYER_H_
