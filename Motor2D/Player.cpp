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
#include "HUD.h"
#include "Animation.h"
#include "InputManager.h"
#include "j1PerfTimer.h"
#include "DialogManager.h"
#include "j1SceneManager.h"
#include "j1EntityManager.h"
#include "Object.h"

Player::Player() : Entity(LINK)
{
	App->inputM->AddListener(this);
}

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
	RELEASE_ARRAY(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		pugi::xml_node attributes;
	
		switch (age)
		{
		default:
		case YOUNG:
			attributes = attributesFile.child("young").child("attributes");
			break;
		case MEDIUM:
			attributes = attributesFile.child("middle").child("attributes");
			break;
		case ADULT:
			attributes = attributesFile.child("adult").child("attributes");
			break;
		}

		LoadAttributes(attributes);

		// base stats
		pugi::xml_node node = attributes.child("base");
		maxLife = life;
		maxStamina = stamina = node.attribute("stamina").as_int(100);
		staminaRec = node.attribute("staminaRec").as_float();

		// attack
		node = attributes.child("attack");
		attackSpeed = node.attribute("speed").as_int(40);
		attackTax = node.attribute("staminaTax").as_int(20);
		spinTax = node.attribute("spinTax").as_int(0);

		node = attributes.child("damaged");
		//damaged
		hitTime = node.attribute("hitTime").as_int(100);
		damagedTime = node.attribute("damagedTime").as_int(2000);
		damagedSpeed = node.attribute("damagedSpeed").as_int(180);

		//dodge
		node = attributes.child("dodge");
		dodgeSpeed = node.attribute("speed").as_int(500);
		dodgeTax = node.attribute("staminaTax").as_int(25);
		dodgeLimit = node.attribute("limit").as_int(50);

	

		invulnerable = false;
		swordCollider = nullptr;
	}

	return ret;
}

bool Player::Update(float dt)
{
	bool ret = true;
	lastPos = currentPos;

	if(dt > 0.1)
	{
		dt = 0;

	}

	if (changeAge > -1 && changeAge < 3)
	{
		ChangeAge((LINK_AGE)changeAge);
		changeAge = -1;		
	}
	
	if (damagedTimer.ReadMs() > damagedTime && invulnerable == true)
	{
		invulnerable = false;
		sprite->tint = { 255, 255, 255, 255 };
	}
	

	ManageStamina(dt);	

	switch (playerState)
	{
	case ACTIVE:
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

		case(DAMAGED):
			Damaged(dt);
			break;

		case (SHOOTING_BOW):
			ShootingBow(dt);
			break;

		case (SPINNING):
			Spinning(dt);
			break;
		}

		break;
	case EVENT:
		Talking(dt);
		break;
	}

	//THRASH FOR VS

	if (attack_vicente && swordCollider != nullptr && actionState == ACTION_STATE::DODGING)
	{
		attack_vicente = false;
		resetSwordCollider();
	}


	return ret;
}

void Player::OnDeath()
{
	
	App->LoadGame("saves.xml");

	App->game->hud->ResetHearts();
	App->game->hud->RestoreHearts();
	damaged = invulnerable = false;
	linearMovement = {0, 0};
	currentDir = D_DOWN;
	sprite->tint = { 255, 255, 255, 255 };
	actionState = IDLE;

	UpdateCollider();

	if (swordCollider != nullptr)
		swordCollider->to_delete = true;

}

void Player::ChangeAge(LINK_AGE new_age)
{
	RELEASE(sprite);
	anim.clear();
	col->to_delete = true;

	// load xml attributes
	pugi::xml_document	attributesFile;
	char* buff;
	int size = App->fs->Load("attributes/player_attributes.xml", &buff);
	pugi::xml_parse_result result = attributesFile.load_buffer(buff, size);
	RELEASE_ARRAY(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
	}
	else
	{
		age = new_age;
		pugi::xml_node attributes;

		switch (age)
		{
		default:
		case YOUNG:
			attributes = attributesFile.child("young").child("attributes");
			break;
		case MEDIUM:
			attributes = attributesFile.child("middle").child("attributes");
			break;
		case ADULT:
			attributes = attributesFile.child("adult").child("attributes");
			break;
		}

		LoadAttributes(attributes);

		// base stats
		pugi::xml_node node = attributes.child("base");
		maxLife = life;
		maxStamina = stamina = node.attribute("stamina").as_int(100);
		staminaRec = node.attribute("staminaRec").as_float();

		// attack
		node = attributes.child("attack");
		attackSpeed = node.attribute("speed").as_int(40);
		attackTax = node.attribute("staminaTax").as_int(20);

		node = attributes.child("damaged");
		//damaged
		hitTime = node.attribute("hitTime").as_int(100);
		damagedTime = node.attribute("damagedTime").as_int(2000);
		damagedSpeed = node.attribute("damagedSpeed").as_int(180);
		//dodge
		node = attributes.child("dodge");
		dodgeSpeed = node.attribute("speed").as_int(500);
		dodgeTax = node.attribute("staminaTax").as_int(25);
		dodgeLimit = node.attribute("limit").as_int(50);
	}

}

void Player::Change_direction()
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		currentDir = D_UP;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		currentDir = D_DOWN;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
		currentDir = D_RIGHT;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
		currentDir = D_LEFT;
}

void Player::ManageStamina(float dt)
{
	if (stamina < 0)
		stamina = 0;
	if (stamina < maxStamina)
	{
		stamina += staminaRec*dt;
	}
	else stamina = maxStamina;
}

bool Player::Idle()
{
	if (damaged == true)
	{
		actionState = DAMAGED;
		//LOG("LINK DAMAGED");
		return true;
	}


	//Walk
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT ||
		App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT ||
		App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT ||
		App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		actionState = WALKING;
		return true;
	}

	if (App->inputM->EventPressed(INPUTEVENT::MUP) == EVENTSTATE::E_REPEAT ||
		App->inputM->EventPressed(INPUTEVENT::MDOWN) == EVENTSTATE::E_REPEAT ||
		App->inputM->EventPressed(INPUTEVENT::MLEFT) == EVENTSTATE::E_REPEAT ||
		App->inputM->EventPressed(INPUTEVENT::MRIGHT) == EVENTSTATE::E_REPEAT)
	{
		actionState = WALKING;
		return true;
	}

	//Spin
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && age == YOUNG && (stamina - spinTax >= 0))
	{
		stamina -= spinTax;
		actionState = SPINNING;
		createSwordCollider();
	}


	//Attack
	else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN||
		App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		if (toTalk != nullptr)
		{
			playerState = EVENT;
		}
		else
		{
			stamina -= attackTax;
			Change_direction();
			actionState = ATTACKING;
			createSwordCollider();
		}
	}

	
	return false;
}

bool Player::Walking(float dt)
{
	bool moving = false;
	dodgeDir = { 0, 0 };

	if (damaged == true)
	{
		actionState = DAMAGED;
	//	LOG("LINK DAMAGED");
		return true;
	}
	

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
		return true;
	}

	/*if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && (stamina - dodgeTax >=0))
	{	
		stamina -= dodgeTax;
		actionState = DODGING;
		Change_direction();
		dodging = true;
		dodgeTimer.Start();
		return true;
	}*/

	//Spin
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && age == YOUNG && (stamina - spinTax >= 0))
	{
		stamina -= spinTax;
		actionState = SPINNING;
		createSwordCollider();
	}
	 

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
			stamina -= attackTax;
			Change_direction();
			actionState = ATTACKING;
			createSwordCollider();
	}

	Change_direction();

	return false;
}

bool Player::Attacking(float dt)
{
	if (damaged == true)
	{
		resetSwordCollider();
		actionState = DAMAGED;
		LOG("LINK DAMAGED");
		return true;
	}

	else if (toTalk != nullptr)
	{
		resetSwordCollider();
		currentAnim->Reset();
		LOG("LINK is in IDLE");
		actionState = IDLE;
		playerState = EVENT;
		toTalk->LookToPlayer();
		return true;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		Move(0, SDL_ceil(attackSpeed * dt));
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		Move(0, -SDL_ceil(attackSpeed * dt));
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		Move(-SDL_ceil(attackSpeed * dt), 0);
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Move(SDL_ceil(attackSpeed * dt), 0);
	}


	if (App->inputM->EventPressed(INPUTEVENT::MDOWN) == EVENTSTATE::E_REPEAT)
	{
		Move(0, SDL_ceil(attackSpeed * dt));
	}
	else if (App->inputM->EventPressed(INPUTEVENT::MUP) == EVENTSTATE::E_REPEAT)
	{
		Move(0, -SDL_ceil(attackSpeed * dt));
	}

	if (App->inputM->EventPressed(INPUTEVENT::MLEFT) == EVENTSTATE::E_REPEAT)
	{
		Move(-SDL_ceil(attackSpeed * dt), 0);
	}
	else if (App->inputM->EventPressed(INPUTEVENT::MRIGHT) == EVENTSTATE::E_REPEAT)
	{
		Move(SDL_ceil(attackSpeed * dt), 0);
	}

	updateSwordCollider();

	if (currentAnim->isOver())
	{
		resetSwordCollider();
		currentAnim->Reset();
		actionState = IDLE;
	}


	return true;
}

bool Player::Dodging(float dt)
{

	Move(SDL_ceil(dodgeSpeed * dodgeDir.x * dt), SDL_ceil(dodgeSpeed*dodgeDir.y* dt));

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		stamina -= attackTax;
		Change_direction();
		actionState = ATTACKING;
		createSwordCollider();
		invulnerable = false;
		LOG("LINK is ATTACKING");
		return true;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		dodging = true;
	}

	if (dodgeTimer.ReadMs() > dodgeLimit)
	{
		if (dodging)
		{
			dodgeTimer.Start();
   			dodging = false;
		}
		else
		{
			actionState = IDLE;
		}
	}
	
	return true;
}

bool Player::Damaged(float dt)
{
	if (damagedTimer.ReadMs() > hitTime)
	{
		actionState = IDLE;
		damaged = false;
		sprite->tint = { 255, 255, 255, 100 };
	}
		
	Move(SDL_ceil(linearMovement.x*damagedSpeed*dt), SDL_ceil(linearMovement.y*damagedSpeed*dt));
	
	return true;
}

bool Player::ShootingBow(float dt)
{
	if (currentAnim->isOver())
	{
		currentAnim->Reset();
		actionState = IDLE;
		iPoint pos = App->map->WorldToMap(currentPos.x, currentPos.y);
		App->game->em->CreateObject(1, pos.x, pos.y, LINK_ARROW);
	}
	return true;
}

bool Player::Spinning(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		Move(0, SDL_ceil(attackSpeed * dt));
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		Move(0, -SDL_ceil(attackSpeed * dt));
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		Move(-SDL_ceil(attackSpeed * dt), 0);
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Move(SDL_ceil(attackSpeed * dt), 0);
	}


	if (App->inputM->EventPressed(INPUTEVENT::MDOWN) == EVENTSTATE::E_REPEAT)
	{
		Move(0, SDL_ceil(attackSpeed * dt));
	}
	else if (App->inputM->EventPressed(INPUTEVENT::MUP) == EVENTSTATE::E_REPEAT)
	{
		Move(0, -SDL_ceil(attackSpeed * dt));
	}

	if (App->inputM->EventPressed(INPUTEVENT::MLEFT) == EVENTSTATE::E_REPEAT)
	{
		Move(-SDL_ceil(attackSpeed * dt), 0);
	}
	else if (App->inputM->EventPressed(INPUTEVENT::MRIGHT) == EVENTSTATE::E_REPEAT)
	{
		Move(SDL_ceil(attackSpeed * dt), 0);
	}

	updateSwordCollider();

	if (currentAnim->isOver())
	{
		resetSwordCollider();
		currentAnim->Reset();
		actionState = IDLE;
	}
	
	return true;
}

bool Player::Talking(float dt)
{
	
	actionState = IDLE;
	App->dialog->text_on_screen->Set_Active_state(true);
	if (firstText == true)
	{
		App->dialog->BlitDialog(toTalk->npcId, toTalk->dialogState);
		firstText = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		if (toTalk != nullptr)
		{
			App->dialog->dialogueStep++;
			if (App->dialog->BlitDialog(toTalk->npcId, toTalk->dialogState) == false)
			{
			playerState = ACTIVE;
			App->dialog->text_on_screen->Set_Active_state(false);
			if (toTalk->dialogState == 0)
			{
				toTalk->dialogState++;
			}
			App->dialog->dialogueStep = 0;
			int test = toTalk->dialogState;
			firstText = true;
			toTalk = nullptr;
			}
		}
	}

	

	return false;
}

void Player::OnInputCallback(INPUTEVENT action, EVENTSTATE state)
{

	switch (action)
	{

	case ATTACK_UP:

		switch (state)
		{
		case E_REPEAT:
			break;

		case E_DOWN:
			if (toTalk != nullptr)
				NextDialog();
			
			else
			{
				if (actionState != ATTACKING && playerState != EVENT)
				{
					if (stamina - attackTax >= 0)
					{
						actionState = ATTACKING;
						currentDir = DIRECTION::D_UP;
						createSwordCollider();
						stamina -= attackTax;
						App->game->hud->stamina_bar->WasteStamina(attackTax);
						attack_vicente = true;
						LOG("LINK is ATTACKING");
					}
					break;
				}
			}
		}
		break;

	case ATTACK_DOWN:
		switch (state)
		{
		case E_REPEAT:
			break;

		case E_DOWN:
			if (toTalk != nullptr)
				NextDialog();
			else
			{
				if (actionState != ATTACKING && playerState != EVENT)
				{
					if (stamina - attackTax >= 0)
					{
						actionState = ATTACKING;
						currentDir = DIRECTION::D_DOWN;
						createSwordCollider();
						stamina -= attackTax;
						App->game->hud->stamina_bar->WasteStamina(attackTax);
						attack_vicente = true;
						LOG("LINK is ATTACKING");
					}
					break;
				}
			}
		}
		break;

	case ATTACK_LEFT:
		switch (state)
		{
		case E_REPEAT:
			break;

		case E_DOWN:
			if (toTalk != nullptr)
				NextDialog();
			else
			{
				if (actionState != ATTACKING && playerState != EVENT)
				{
					if (stamina - attackTax >= 0)
					{
						actionState = ATTACKING;
						currentDir = DIRECTION::D_LEFT;
						createSwordCollider();
						stamina -= attackTax;
						App->game->hud->stamina_bar->WasteStamina(attackTax);
						attack_vicente = true;
						LOG("LINK is ATTACKING");
					}
					break;
				}
			}
		}
		break;

	case ATTACK_RIGHT:
		switch (state)
		{
		case E_REPEAT:
			break;

		case E_DOWN:
			if (toTalk != nullptr)
				NextDialog();
			else
			{
				if (actionState != ATTACKING && playerState != EVENT)
				{
					if (stamina - attackTax >= 0)
					{
						actionState = ATTACKING;

						currentDir = DIRECTION::D_RIGHT;
						createSwordCollider();
						stamina -= attackTax;
						App->game->hud->stamina_bar->WasteStamina(attackTax);
						attack_vicente = true;
						LOG("LINK is ATTACKING");
					}
					break;
				}
			}
		}
		break;

	case DODGE:
		if (state == E_DOWN && (stamina - dodgeTax >= 0))
		{
			stamina -= dodgeTax;
			App->game->hud->stamina_bar->WasteStamina(dodgeTax);
			actionState = DODGING;
			Change_direction();
			dodging = true;
			dodgeTimer.Start();
		}
		break;

	case UP:
	{
		
	}

	case DOWN:
	{
	
	}

	}
}

void Player::createSwordCollider()
{
	if(actionState == ATTACKING)
		switch (currentDir)
	{
	case(D_UP):
		swordCollider = App->collisions->AddCollider({ currentPos.x, currentPos.y, 26, 14 }, COLLIDER_LINK_SWORD);
		break;
	case(D_DOWN):
		swordCollider = App->collisions->AddCollider({ currentPos.x, currentPos.y, 26, 14 }, COLLIDER_LINK_SWORD);
		break;
	case(D_RIGHT):
		swordCollider = App->collisions->AddCollider({ currentPos.x, currentPos.y, 14, 26 }, COLLIDER_LINK_SWORD);
		break;
	case(D_LEFT):
		swordCollider = App->collisions->AddCollider({ currentPos.x, currentPos.y, 14, 26 }, COLLIDER_LINK_SWORD);
		break;
	}

	else if (actionState == SPINNING)
	{
		swordCollider = App->collisions->AddCollider({ currentPos.x, currentPos.y, 40, 40 }, COLLIDER_LINK_SWORD);
	}
	updateSwordCollider();
}

void Player::updateSwordCollider()
{
	

	if (actionState == ATTACKING)
	switch (currentDir)
	{
	case(D_UP):
		swordCollider->rect.x = currentPos.x - swordCollider->rect.w/2;
		swordCollider->rect.y = currentPos.y - swordCollider->rect.h*2;
		break;

	case(D_DOWN):
		swordCollider->rect.x = currentPos.x - swordCollider->rect.w / 2;
		swordCollider->rect.y = currentPos.y;
		break;

	case(D_RIGHT):
		swordCollider->rect.y = currentPos.y - 16;
		swordCollider->rect.x = currentPos.x + 5;
		break;

	case(D_LEFT):
		swordCollider->rect.y = currentPos.y - 16;
		swordCollider->rect.x = currentPos.x - swordCollider->rect.w -  5;
		break;
	}

	else if (actionState == SPINNING)
	{
		swordCollider->rect.x = currentPos.x - swordCollider->rect.w / 2;
		swordCollider->rect.y = currentPos.y - swordCollider->rect.h / 2 -10;
	}

}

void Player::resetSwordCollider()
{
	if (swordCollider != nullptr)
	{
		swordCollider->to_delete = true;
		swordCollider = nullptr;
	}
}

void Player::NextDialog()
{
	if (toTalk != nullptr)
	{
		App->dialog->dialogueStep++;
		if (App->dialog->BlitDialog(toTalk->npcId, toTalk->dialogState) == false)
		{
			playerState = ACTIVE;
			App->dialog->text_on_screen->Set_Active_state(false);
			if (toTalk->dialogState == 0)
			{
				toTalk->dialogState++;
			}
			App->dialog->dialogueStep = 0;
			int test = toTalk->dialogState;
			firstText = true;
			toTalk = nullptr;
		}
	}
}
