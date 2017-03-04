#include "Player.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "p2Log.h"

Player::Player()
{
	worldPosition = iPoint(150, 150);
	mapPosition = iPoint(50, 50);
}

bool Player::Awake(pugi::xml_node& config)
{
	bool ret = true;
	speed = 300;

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

bool Player::UpdateTicks()
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

bool Player::Idle()
{
	Change_direction();

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		player_state = WALKING;
		LOG("Link is WALKING");
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
		worldPosition.y += SDL_ceil(speed * dt);
		moving = true;
	}
	
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		current_direction = D_UP;
		worldPosition.y -= SDL_ceil(speed * dt);
		moving = true;
	}
	
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		current_direction = D_LEFT;
		worldPosition.x -= SDL_ceil(speed * dt);
		moving = true;

	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		current_direction = D_RIGHT;
		worldPosition.x += SDL_ceil(speed * dt);
		moving = true;

	}
	
	if(moving == false)
	{
		player_state = IDLE;
		LOG("Link is in IDLE");
		return true;
	}

	Change_direction();

	return false;
}


