#include "Player.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "p2Log.h"

Player::Player()
{
} 

bool Player::Awake(pugi::xml_node& config)
{
	bool ret = true;

	//TODO: PICK THESE DATA FROM XML

	worldPosition = iPoint(150, 150);
	mapPosition = iPoint(50, 50);

	speed = 70;
	attacking_speed = 40;
	col = App->collisions->AddCollider({ worldPosition.x, worldPosition.y, 16, 15 }, COLLIDER_PLAYER, ((j1Module*)App->game));
	collider_pivot = { 8, 12 };

	return ret;
}

bool Player::Start()
{
	bool ret = true;

	if (ret = loadAnimations())
	{
		playerTex = App->tex->Load("textures/Link_sprites/Link_young.png");
		current_animation = &playerAnim.find({ IDLE, D_DOWN })->second;
		sprite = new Sprite(playerTex, worldPosition, SCENE, current_animation->getCurrentFrame(), current_animation->pivot);
		player_state = IDLE;
	}

	return ret;
}

bool Player::PreUpdate()
{
	bool ret = true;

	return ret;
}

bool Player::Update(float dt)
{
	bool ret = true;


	switch (player_state)
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
	}
	return ret;
}

bool Player::PostUpdate()
{
	bool ret = true;

	// draw
	current_animation = &playerAnim.find({ player_state, current_direction })->second;

	sprite->updateSprite(worldPosition, current_animation->pivot, current_animation->getCurrentFrame(), current_animation->flip);
	
	App->render->Draw(sprite);

	return ret;
}

bool Player::CleanUp()
{
	bool ret = true;

	return ret;
}

iPoint Player::GetWorldPosition()
{
	return worldPosition;
}

bool Player::loadAnimations()
{
	bool ret = true;

	pugi::xml_document	anim_file;
	pugi::xml_node		anim;
	char* buff;
	int size = App->fs->Load("animations/player_animations.xml", &buff);
	pugi::xml_parse_result result = anim_file.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load animation xml file. Pugi error: %s", result.description());
		ret = false;
	}
	else
		anim = anim_file.child("animations");

	if (ret == true)
	{
		pugi::xml_node ent = anim.child("LINK");

		for (pugi::xml_node action = ent.child("IDLE"); action != NULL; action = action.next_sibling())
		{
			for (pugi::xml_node dir = action.child("UP"); dir != action.child("loop"); dir = dir.next_sibling())
			{
				std::pair<ACTION_STATE, DIRECTION> p;
				int state = action.child("name").attribute("value").as_int();
				p.first = (ACTION_STATE)state;

				int di = dir.first_child().attribute("name").as_int();
				p.second = (DIRECTION)di;

				Animation anims;
				int x = dir.first_child().attribute("x").as_int();
				int y = dir.first_child().attribute("y").as_int();
				int w = dir.first_child().attribute("w").as_int();
				int h = dir.first_child().attribute("h").as_int();
				int fN = dir.first_child().attribute("frameNumber").as_int();
				int margin = dir.first_child().attribute("margin").as_int();
				bool loop = action.child("loop").attribute("value").as_bool();
				int pivotX = dir.first_child().attribute("pivot_x").as_int();
				int pivotY = dir.first_child().attribute("pivot_y").as_int();
				int flip = dir.first_child().attribute("flip").as_int();
				float animSpeed = action.child("speed").attribute("value").as_float();

				anims.setAnimation(x, y, w, h, fN, margin);
				anims.loop = loop;
			
				anims.speed = animSpeed;
				anims.pivot.x = pivotX;
				anims.pivot.y = pivotY;

				if (flip == 1)
					anims.flip = SDL_FLIP_HORIZONTAL;
				else if (flip == 2)
					anims.flip = SDL_FLIP_VERTICAL;
				else anims.flip = SDL_FLIP_NONE;


				iPoint piv;

				playerAnim.insert(std::pair<std::pair<ACTION_STATE, DIRECTION>, Animation >(p, anims));
				playerAnim.find({ p.first, p.second })->second.pivot.Set(pivotX, pivotY);
				piv = playerAnim.find({ p.first, p.second })->second.pivot;
			}
		}
	}


	return ret;
}

void Player::Change_direction()
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		current_direction = D_UP;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		current_direction = D_DOWN;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		current_direction = D_RIGHT;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		current_direction = D_LEFT;
}

//Displace the entity a given X and Y taking in account collisions w/map
void Player::Move(int x, int y)
{
	worldPosition.x += x;
	UpdateCollider();
	if(col->CheckMapCollision())
		worldPosition.x -= x;

	worldPosition.y += y;
	UpdateCollider();
	if (col->CheckMapCollision())
		worldPosition.y -= y;
}


void Player::UpdateCollider()
{
	col->rect.x = worldPosition.x - collider_pivot.x;
	col->rect.y = worldPosition.y - collider_pivot.y;
}



bool Player::Idle()
{
	Change_direction();

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		player_state = WALKING;
		LOG("Link is WALKING");
		return true;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		Change_direction();
		player_state = ATTACKING;
		LOG("LINK is ATTACKING");
		return true;
	}

	return false;
}

bool Player::Walking(float dt)
{
	bool moving = false;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		current_direction = D_DOWN;
		Move(0, SDL_ceil(speed * dt));
		moving = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		current_direction = D_UP;
		Move(0, -SDL_ceil(speed * dt));
		moving = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		current_direction = D_LEFT;
		Move(-SDL_ceil(speed * dt), 0);
		moving = true;

	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		current_direction = D_RIGHT;
		Move(SDL_ceil(speed * dt), 0);
		moving = true;

	}

	if (moving == false)
	{
		player_state = IDLE;
		LOG("Link is in IDLE");
		return true;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		Change_direction();
		player_state = ATTACKING;
		LOG("LINK is ATTACKING");
		return true;
	}

	Change_direction();

	return false;
}

bool Player::Attacking(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		Move(0, SDL_ceil(attacking_speed * dt));
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		Move(0, -SDL_ceil(attacking_speed * dt));
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		Move(-SDL_ceil(attacking_speed * dt), 0);
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Move(SDL_ceil(attacking_speed * dt), 0);
	}

	if (current_animation->isOver())
	{
		current_animation->Reset();
		LOG("LINK is in IDLE");
		player_state = IDLE;
	}

	return true;
}