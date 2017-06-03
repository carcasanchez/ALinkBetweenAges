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
#include "Brofiler\Brofiler.h"
#include "j1Audio.h"
#include "j1QuestManager.h"
#include "Player.h"


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

		//max items
		maxArrows = 30;
		maxBombs = 10;
		maxLifePotions = 1;
		maxStaminaPotions = 1;
		invulnerable = false;
		swordCollider = nullptr;
	}

	return ret;
}
void Player::OnDeath()
{
	App->audio->PlayFx(4);
	App->LoadGame("saves.xml");
	damaged = invulnerable = false;
	linearMovement = { 0, 0 };
	sprite->tint = { 255, 255, 255, 255 };
	actionState = IDLE;
	dead = true;
	col->active = false;

	UpdateCollider();

	if (swordCollider != nullptr)
		swordCollider->to_delete = true;

}
bool Player::Update(float dt)
{
	bool ret = true;
	lastPos = currentPos;

	if(dt > 0.1)
		dt = 0;

	if (changeAge > -1 && changeAge < 3)
	{
		ChangeAge((LINK_AGE)changeAge);
		changeAge = -1;		
	}
	
	if (damagedTimer.ReadMs() > damagedTime)
	{
		invulnerable = false;
		sprite->tint = { 255, 255, 255, 255 };
	}
	

	ManageStamina(dt);	

	ShowPickedObject();

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN)
		if (equippedObject >= inventory.size() - 1)
			equippedObject = 0;

		else equippedObject++;

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
	{
				UseObject();
	}
	else if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_UP)
	{
		holded_item = NO_OBJECT;
	}

	

	
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

		case (JUMPING):
			Jumping(dt);
			break;
		}

		break;
	case EVENT:
		Talking(dt);
		break;
	case CUTSCENE:
		break;
	}

	return ret;
}




//State machine
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

	

	//Attack
	else if ((App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN))
	{

		if (holded_item == BOW && arrows > 0)
		{
			App->audio->PlayFx(18);
			arrows--;
			actionState = SHOOTING_BOW;
			Change_direction();
		}
		else if (stamina - attackTax > 0)
		{
			if (toTalk != nullptr)
				playerState = EVENT;
			stamina -= attackTax;
			Change_direction();
			actionState = ATTACKING;
			createSwordCollider();
			App->audio->PlayFx(1);
		}
	}

	//Dodge
	/*switch (currentDir)
	{
	case D_UP:
		dodgeDir.y = -1;
		break;
	case D_DOWN:
		dodgeDir.y = 1;
		break;
	case D_RIGHT:
		dodgeDir.x = 1;
		break;
	case D_LEFT:
		dodgeDir.x = -1;
		break;
	}
	*/
	if(App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_DOWN)
		if (actionState != SPINNING && actionState != ATTACKING &&
			actionState != JUMPING && stamina > spinTax && 
			App->game->em->player->ableToSpin == true)
		{
		App->audio->PlayFx(1);
		stamina -= spinTax;
		actionState = SPINNING;
		createSwordCollider();
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


	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && (stamina - dodgeTax >= 0))
	{
		App->audio->PlayFx(25);
		stamina -= dodgeTax;
		App->game->hud->stamina_bar->WasteStamina(dodgeTax);
		actionState = DODGING;
		Change_direction();
		dodging = true;
		dodgeTimer.Start();
	}
	
	//Jump
	if (!toJump.IsZero())
	{
		actionState = JUMPING;
		jumpOrigin = App->map->WorldToMap(currentPos.x, currentPos.y);

		if (toJump.y > jumpOrigin.y)
			currentDir = D_DOWN;
		else if (toJump.x > jumpOrigin.x)
			currentDir = D_RIGHT;
		else if (toJump.x < jumpOrigin.x)
			currentDir = D_LEFT;
		forceUp = 4;
		forceDown = 0;
		jumpTimer.Start();

		return true;
	}
	 

	//Attack
	if ((App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN ))
	{

		if (holded_item == BOW && arrows > 0)
		{
			App->audio->PlayFx(18);
			arrows--;
			actionState = SHOOTING_BOW;
		}
		else if(stamina - attackTax > 0)
		{ 
			stamina -= attackTax;
			Change_direction();
			actionState = ATTACKING;
			createSwordCollider();
			App->audio->PlayFx(1);
		}
	}

	else if (App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_DOWN)
		if (actionState != SPINNING && actionState != ATTACKING &&
			actionState != JUMPING && stamina > spinTax &&
			App->game->em->player->ableToSpin == true)
		{
			App->audio->PlayFx(1);
			stamina -= spinTax;
			actionState = SPINNING;
			createSwordCollider();
		}

	

	Change_direction();

	return true;
}

bool Player::Attacking(float dt)
{
	if (damaged == true)
	{
		resetSwordCollider();
		actionState = DAMAGED;
		return true;
	}

	else if (toTalk != nullptr)
	{
		if (!App->quest->TriggerTalkToCallback(toTalk))
			App->quest->StepTalkToCallback(toTalk);

		toTalk->LookToPlayer();
		resetSwordCollider();
		currentAnim->Reset();

		if (toTalk->dialogState != -1)
		{
			actionState = IDLE;
			playerState = EVENT;
		}

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
		Object* arrow = App->game->em->ActiveObject(currentPos.x-5, currentPos.y-6, LINK_ARROW, currentDir);
		if (arrow->currentDir == D_RIGHT || arrow->currentDir == D_LEFT)
		{
			arrow->col->rect.h = 7;
			arrow->col->rect.w = 15;
		}
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

bool Player::Jumping(float dt)
{
	dodging = true;
	iPoint mapPos = App->map->WorldToMap(currentPos.x, currentPos.y);

	if ((currentDir == D_DOWN && mapPos.y >= toJump.y)||
		(currentDir == D_RIGHT && mapPos.x >= toJump.x)||
		(currentDir == D_LEFT && mapPos.x <= toJump.x))
	{
		forceUp = forceDown = 0;
		actionState = IDLE;
		dodging = false;
		toJump.SetToZero();
		return true;
	}

	
	if (currentDir == D_RIGHT || currentDir == D_LEFT)
	{

		int totalTime = (toJump.x - jumpOrigin.x) / (speed * dt * 4);

		if (currentDir = D_RIGHT)
		{		
			currentPos.x += (speed * dt * 4);			
		}
		else
		{
			currentPos.x -= (speed * dt * 4);
		}
		
	}
	else if (currentDir == D_DOWN)
	{
		currentDir = D_DOWN;
		currentPos.y -= SDL_ceil(speed * dt * forceUp);
		currentPos.y += SDL_ceil(speed * dt * forceDown);
		if(forceDown < 10)
			forceDown += 30 * dt;
	}

	return true;
}

bool Player::Talking(float dt)
{
	
	actionState = IDLE;
	App->dialog->text_on_screen->Set_Active_state(true);
	if (firstText == true)
	{
		App->dialog->BlitDialog(toTalk->id, toTalk->dialogState);
		firstText = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		if (toTalk != nullptr)
		{
			if (App->dialog->text_on_screen->dialog_state == MID_TEXT)
				App->dialog->text_on_screen->ForcedFinish();

			else
			{
				App->dialog->dialogueStep++;
				if (App->dialog->BlitDialog(toTalk->id, toTalk->dialogState) == false)
				{
					playerState = ACTIVE;
					App->dialog->text_on_screen->Set_Active_state(false);
					if (toTalk->dialogState == 0 && App->dialog->NumDialog(toTalk->id) == 2)
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
	}

	

	return false;
}

void Player::GetHit(Entity * agressor)
{

	if (!agressor->col)
		return;

	damaged = invulnerable = true;
	damagedTimer.Start();

	

	if (agressor->col->type == COLLIDER_BOMB_EXPLOSION)
		life -= 1;
	else
		life -= agressor->damage;
	
	sprite->tint = { 100, 0, 0, 255 };

	if (col->rect.x < agressor->col->rect.x)
		linearMovement.x = -1;
	else linearMovement.x = 1;

	if (col->rect.y < agressor->col->rect.y)
		linearMovement.y = -1;
	else linearMovement.y = 1;

	App->audio->PlayFx(5);
}

void Player::ShowPickedObject()
{
	if (pickedObject)
	{
		pickedObject->currentPos = currentPos;
		pickedObject->currentPos.y -= 30;

		if (showObjectTimer.ReadMs() > 500)
		{
			pickedObject->life = -1;
			pickedObject = nullptr;
		}
	}	

}




void Player::OnInputCallback(INPUTEVENT action, EVENTSTATE state)
{

	switch (action)
	{

	case ATTACK_UP:

		if (state == E_DOWN)
		{
			switch (holded_item)
			{
			case NO_OBJECT:
				if (toTalk != nullptr)
					NextDialog();

				else
				{
					if (actionState != ATTACKING && actionState != SPINNING && actionState != JUMPING && playerState != EVENT)
					{
						if (stamina - attackTax >= 0)
						{
							App->audio->PlayFx(1);
							actionState = ATTACKING;
							currentDir = DIRECTION::D_UP;
							createSwordCollider();
							stamina -= attackTax;
							App->game->hud->stamina_bar->WasteStamina(attackTax);
						}
						break;
					}
				}
				break;
			
			case BOW:
				if (arrows > 0)
				{
					App->audio->PlayFx(18);
					currentDir = DIRECTION::D_UP;
					arrows--;
					actionState = SHOOTING_BOW;
				}
				break;
			}
		}
		break;

	case ATTACK_DOWN:

		if (state == E_DOWN)
		{
			switch (holded_item)
			{
			case NO_OBJECT:
				if (toTalk != nullptr)
					NextDialog();

				else
				{
					if (actionState != ATTACKING && actionState != SPINNING && actionState != JUMPING &&  playerState != EVENT)
					{
						if (stamina - attackTax >= 0)
						{
							App->audio->PlayFx(1);
							actionState = ATTACKING;
							currentDir = DIRECTION::D_DOWN;
							createSwordCollider();
							stamina -= attackTax;
							App->game->hud->stamina_bar->WasteStamina(attackTax);
						}
						break;
					}
				}
				break;
		
			case BOW:
				if (arrows > 0)
				{
					App->audio->PlayFx(18);
					currentDir = DIRECTION::D_DOWN;
					arrows--;
					actionState = SHOOTING_BOW;
				}
				break;
			}
		}
		break;

	case ATTACK_LEFT:

		if (state == E_DOWN)
		{
			switch (holded_item)
			{
			case NO_OBJECT:
				if (toTalk != nullptr)
					NextDialog();

				else
				{
					if (actionState != ATTACKING && actionState != SPINNING && actionState != JUMPING && playerState != EVENT)
					{
						if (stamina - attackTax >= 0)
						{
							App->audio->PlayFx(1);

							actionState = ATTACKING;
							currentDir = DIRECTION::D_LEFT;
							createSwordCollider();
							stamina -= attackTax;
							App->game->hud->stamina_bar->WasteStamina(attackTax);
						}
						break;
					}
				}
				break;
			case BOMB_SAC:
				if (bombs > 0)
				{
					bombs--;
					App->game->em->ActiveObject(currentPos.x, currentPos.y, BOMB);
				}
				break;

			case BOW:
				if (arrows > 0)
				{
					App->audio->PlayFx(18);
					currentDir = DIRECTION::D_LEFT;
					arrows--;
					actionState = SHOOTING_BOW;
				}
				
				break;
			}
		}
		break;

	case ATTACK_RIGHT:

		if (state == E_DOWN)
		{
			switch (holded_item)
			{
			case NO_OBJECT:
				if (toTalk != nullptr)
					NextDialog();

				else
				{
					if (actionState != ATTACKING && actionState != SPINNING && actionState != JUMPING && playerState != EVENT)
					{
						if (stamina - attackTax >= 0)
						{
							App->audio->PlayFx(1);

							actionState = ATTACKING;
							currentDir = DIRECTION::D_RIGHT;
							createSwordCollider();
							stamina -= attackTax;
							App->game->hud->stamina_bar->WasteStamina(attackTax);
						}
						break;
					}
				}
				break;		
			case BOW:

				if (arrows > 0)
				{
					App->audio->PlayFx(18);
					currentDir = DIRECTION::D_RIGHT;
					arrows--;
					actionState = SHOOTING_BOW;
				}
				break;
			}
		}
		break;


	case DODGE:
		if (state == E_DOWN && (stamina - dodgeTax >= 0))
		{
			if (actionState != SPINNING && actionState != ATTACKING && actionState != JUMPING)
			{
				App->audio->PlayFx(25);

				stamina -= dodgeTax;
				App->game->hud->stamina_bar->WasteStamina(dodgeTax);
				actionState = DODGING;
				Change_direction();
				dodging = true;
				dodgeTimer.Start();
			}
		}
		break;

	case SPIN_TO_WIN:
		if (actionState != SPINNING && actionState != ATTACKING && actionState != JUMPING && stamina > spinTax && App->game->em->player->ableToSpin == true)
		{
			App->audio->PlayFx(1);
			stamina -= spinTax;
			actionState = SPINNING;
			createSwordCollider();
		}
		break;

	case USE_ITEM:

		if(!only_one_time)
			UseObject();

		break;

	case STOP_ITEM:
		holded_item = NO_OBJECT;
		only_one_time = false;
		break;

	case CHANGE_ITEM:
		if (state == E_DOWN)
		{
			if (equippedObject >= inventory.size() - 1)
				equippedObject = 0;

			else equippedObject++;
		}
		break;
	}
}

void Player::createSwordCollider()
{
	if(actionState == ATTACKING)
		switch (currentDir)
	{
	case(D_UP):
		swordCollider = App->collisions->AddCollider({ currentPos.x, currentPos.y, 30, 22 }, COLLIDER_LINK_SWORD);
		break;
	case(D_DOWN):
		swordCollider = App->collisions->AddCollider({ currentPos.x, currentPos.y, 30, 22 }, COLLIDER_LINK_SWORD);
		break;
	case(D_RIGHT):
		swordCollider = App->collisions->AddCollider({ currentPos.x, currentPos.y, 22, 30 }, COLLIDER_LINK_SWORD);
		break;
	case(D_LEFT):
		swordCollider = App->collisions->AddCollider({ currentPos.x, currentPos.y, 22, 30 }, COLLIDER_LINK_SWORD);
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
		swordCollider->rect.y = currentPos.y - swordCollider->rect.h*1.5;
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
		if (App->dialog->text_on_screen->dialog_state == MID_TEXT)
			App->dialog->text_on_screen->ForcedFinish();
		
		else
		{
			App->dialog->dialogueStep++;
			if (App->dialog->BlitDialog(toTalk->id, toTalk->dialogState) == false)
			{
				playerState = ACTIVE;
				App->dialog->text_on_screen->Set_Active_state(false);
				if (toTalk->dialogState == 0 && App->dialog->NumDialog(toTalk->id) == 2)
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
		int tmp = life;
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
	
		maxLife = life + bonusLife;
		life = tmp;
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

void Player::UseObject(float dt)
{
	if (inventory.size() <= 0)
		return;


	std::list<OBJECT_TYPE>::iterator currentItem;
	int i = 0;
	for (currentItem = inventory.begin() ; i < equippedObject ; currentItem++, i++){}


	switch ((*currentItem))
	{
	case LIFE_POTION:		
		if(maxLife-life >=3)
		life += 3;
		else life = maxLife;

		if (equippedObject == inventory.size() - 1)
			equippedObject = 0;

		inventory.erase(currentItem);
		only_one_time = true;
		lifePotions--;
		App->audio->PlayFx(2);
		break;

	case STAMINA_POTION:
		stamina = maxStamina;

		if(equippedObject == inventory.size() - 1)
			equippedObject = 0;
	
		inventory.erase(currentItem);
		only_one_time = true;
		staminaPotions--;
		App->audio->PlayFx(2);
		break;

	case BOMB_SAC:
		if (bombs > 0)
		{
			bombs--;
			App->game->em->ActiveObject(currentPos.x, currentPos.y, BOMB);
		}
		only_one_time = true;
		break;


	case BOW:
		if (arrows > 0)
			holded_item = BOW;
		break;
	}



}
