#include "Player.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1CollisionManager.h"
#include "p2Log.h"
#include "Animation.h"
#include "InputManager.h"

Player::Player() : Entity() { App->inputM->AddListener(this); }

bool Player::Spawn(std::string file, iPoint pos)
{
	bool ret = true;

	// set position
	currentPos = lastPos = pos;

	// load xml attributes
	pugi::xml_document	attributesFile;
	char* buff;
	int size = App->fs->Load(file.c_str(), &buff);
	pugi::xml_parse_result result = attributesFile.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		pugi::xml_node attributes = attributesFile.child("attributes");

		LoadAttributes(attributes);

		// base stats
		pugi::xml_node node = attributes.child("base");
		maxLife = life;
		maxStamina = stamina = node.attribute("stamina").as_int(100);
		staminaRec = node.attribute("staminaRec").as_float();
		speed = node.attribute("speed").as_int(70);

		// attack
		node = attributes.child("attack");
		attackSpeed = node.attribute("speed").as_int(40);
		attackTax = node.attribute("staminaTax").as_int(20);

		//dodge
		node = attributes.child("dodge");
		dodgeSpeed = node.attribute("speed").as_int(500);
		dodgeTax = node.attribute("staminaTax").as_int(25);
		dodgeLimit = node.attribute("limit").as_int(50);
	}

	return ret;
}

bool Player::Update(float dt)
{
	bool ret = true;
	lastPos = currentPos;

	if (stamina < maxStamina)
	{
		stamina += staminaRec;
	}
	else stamina = maxStamina;

	switch (actionState)
	{
		case(IDLE):
			Idle();
			break;

		case(WALKING):
			Walking(dt);
			break;

		case(ATTACKING):
			Attacking(dt);
			break;

		case(DODGING):
			Dodging(dt);
			break;
	}

	UpdateCollider();
	return ret;
}

void Player::Change_direction()
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		currentDir = D_UP;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		currentDir = D_DOWN;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		currentDir = D_RIGHT;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		currentDir = D_LEFT;
}

bool Player::Idle()
{
	Change_direction();

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		actionState = WALKING;
		LOG("Link is WALKING");
		return true;
	}

	if (App->inputM->EventPressed(INPUTEVENT::MUP) == EVENTSTATE::E_REPEAT || App->inputM->EventPressed(INPUTEVENT::MDOWN) == EVENTSTATE::E_REPEAT || App->inputM->EventPressed(INPUTEVENT::MLEFT) == EVENTSTATE::E_REPEAT || App->inputM->EventPressed(INPUTEVENT::MRIGHT) == EVENTSTATE::E_REPEAT)
	{
		actionState = WALKING;
		LOG("Link is WALKING");
		return true;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && (stamina - attackTax >= 0))
	{
		stamina -= attackTax;
		Change_direction();
		actionState = ATTACKING;
		LOG("LINK is ATTACKING");
		return true;
	}

	return false;
}

bool Player::Walking(float dt)
{
	bool moving = false;
	dodgeDir = { 0, 0 };

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
			currentDir = D_DOWN;
			dodgeDir.y = 1;
			Move(0, SDL_ceil(speed * dt));
			moving = true;
		}
		else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			currentDir = D_UP;
			dodgeDir.y = -1;
			Move(0, -SDL_ceil(speed * dt));
			moving = true;
		}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		
		{

		currentDir = D_LEFT;
		dodgeDir.x = -1;
		Move(-SDL_ceil(speed * dt), 0);
		moving = true;

		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			currentDir = D_RIGHT;
			dodgeDir.x = 1;
			Move(SDL_ceil(speed * dt), 0);
			moving = true;
		}

	if (App->inputM->EventPressed(INPUTEVENT::MDOWN) == EVENTSTATE::E_REPEAT)
	{
		currentDir = D_DOWN;
		dodgeDir.y = 1;
		Move(0, SDL_ceil(speed * dt));
		moving = true;
	}
	else if (App->inputM->EventPressed(INPUTEVENT::MUP) == EVENTSTATE::E_REPEAT)
	{
		currentDir = D_UP;
		dodgeDir.y = -1;
		Move(0, -SDL_ceil(speed * dt));
		moving = true;
	}

	if (App->inputM->EventPressed(INPUTEVENT::MLEFT) == EVENTSTATE::E_REPEAT)
	{
		currentDir = D_LEFT;
		dodgeDir.x = -1;
		Move(-SDL_ceil(speed * dt), 0);
		moving = true;

	}
	else if (App->inputM->EventPressed(INPUTEVENT::MRIGHT) == EVENTSTATE::E_REPEAT)
	{
		currentDir = D_RIGHT;
		dodgeDir.x = 1;
		Move(SDL_ceil(speed * dt), 0);
		moving = true;
	}



	if (moving == false)
	{
		actionState = IDLE;
		LOG("Link is in IDLE");
		return true;
	}

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN && (stamina- dodgeTax >=0))
	{	
		stamina -= dodgeTax;
		LOG("LINK is DODGING");
		Change_direction();
		actionState = DODGING;
		dodgeTimer.Start();
		return true;
	}
	 
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && (stamina - attackTax >= 0))
	{
 		stamina -= attackTax;
		Change_direction();
		actionState = ATTACKING;
		LOG("LINK is ATTACKING");
		return true;
	}

	Change_direction();

	return false;
}

bool Player::Attacking(float dt)
{
	if (currentDir == D_DOWN)
	{
		Move(0, SDL_ceil(attackSpeed * dt));
	}
	else if (currentDir == D_UP)
	{
		Move(0, -SDL_ceil(attackSpeed * dt));
	}

	if (currentDir == D_LEFT)
	{
		Move(-SDL_ceil(attackSpeed * dt), 0);
	}
	else if (currentDir == D_RIGHT)
	{
		Move(SDL_ceil(attackSpeed * dt), 0);
	}

	if (currentAnim->isOver())
	{
		currentAnim->Reset();
		LOG("LINK is in IDLE");
		actionState = IDLE;
	}

	return true;
}

bool Player::Dodging(float dt)
{
	if (dodgeTimer.ReadMs() > dodgeLimit)
	{
		actionState = IDLE;
	}

	Move(SDL_ceil(dodgeSpeed * dodgeDir.x * dt), SDL_ceil(dodgeSpeed*dodgeDir.y* dt));
	
	return true;
}

void Player::OnInputCallback(INPUTEVENT action, EVENTSTATE state)
{
	if (action == ATTACK)
	{
		switch (state)
		{
		case E_REPEAT:
			break;

		case E_DOWN:
			if (stamina - attackTax >= 0)
			{
				stamina -= attackTax;
				actionState = ATTACKING;
				LOG("LINK is ATTACKING");
			}
			break;
		}
	}

	if (action == DODGE)
	{
		if (state == E_DOWN && (stamina - dodgeTax >= 0))
		{
			stamina -= dodgeTax;
			LOG("LINK is DODGING");
			actionState = DODGING;
			dodgeTimer.Start();
		}
	}

}
