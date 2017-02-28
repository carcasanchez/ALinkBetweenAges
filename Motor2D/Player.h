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
	BASIC_ATTACK,
	DEATH,
	RUNNING
};

enum DIRECTION
{
	D_BACK,
	D_BACK_RIGHT,
	D_RIGHT,
	D_FRONT_RIGHT,
	D_FRONT,
	D_FRONT_LEFT,
	D_LEFT,
	D_BACK_LEFT,
	D_DEFAULT
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

private:

	bool loadAnimations();

private:

	//PlayerAttributes* attributes = NULL;
	iPoint worldPosition;	// position in pixels of player // This is the point of the pivot in the world //The feet of the player
	iPoint mapPosition;		// position of the tile where player is


	Sprite* sprite = NULL;
	SDL_Texture* playerTex;

	std::map<std::pair<ACTION_STATE, DIRECTION>, Animation>	playerAnim;
	Animation* current_animation;
	ACTION_STATE current_action = IDLE;
	DIRECTION current_direction = D_FRONT;
};

#endif // !_PLAYER_H_
