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

	return ret;
}

bool Player::Start()
{
	bool ret = true;

	if (ret = loadAnimations())
	{
		playerTex = App->tex->Load("textures/Barbarian.png");
		current_animation = &playerAnim.find({ IDLE, D_FRONT })->second;
		sprite = new Sprite(playerTex, worldPosition, SCENE, current_animation->getCurrentFrame(), current_animation->pivot);
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

	current_action = WALKING;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		worldPosition.y++;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			current_direction = D_FRONT_LEFT;
			worldPosition.x--;
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			worldPosition.x++;
			current_direction = D_FRONT_RIGHT;
		}
		else
		{
			current_direction = D_FRONT;
		}

	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		worldPosition.y--;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			worldPosition.x--;
			current_direction = D_BACK_LEFT;
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			worldPosition.x++;
			current_direction = D_BACK_RIGHT;
		}
		else
		{
			current_direction = D_BACK;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		worldPosition.x--;
		current_direction = D_LEFT;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		worldPosition.x++;
		current_direction = D_RIGHT;
	}
	else
	{
		current_action = IDLE;
	}

	return ret;
}

bool Player::PostUpdate()
{
	bool ret = true;

	// draw
	current_animation = &playerAnim.find({ current_action, current_direction })->second;

	sprite->updateSprite(worldPosition, current_animation->pivot, current_animation->getCurrentFrame());

	App->render->Draw(sprite);

	return ret;
}

bool Player::CleanUp()
{
	bool ret = true;

	return ret;
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
		pugi::xml_node ent = anim.child("BARBARIAN");

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
				float animSpeed = action.child("speed").attribute("value").as_float();
				anims.setAnimation(x, y, w, h, fN, margin);
				anims.loop = loop;
				anims.speed = animSpeed;
				anims.pivot.x = pivotX;
				anims.pivot.y = pivotY;
				iPoint piv;

				playerAnim.insert(std::pair<std::pair<ACTION_STATE, DIRECTION>, Animation >(p, anims));
				playerAnim.find({ p.first, p.second })->second.pivot.Set(pivotX, pivotY);
				piv = playerAnim.find({ p.first, p.second })->second.pivot;
			}
		}
	}


	return ret;
}