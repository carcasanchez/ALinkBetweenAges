#include "Entity.h"
#include "Animation.h"
#include "j1Render.h"
#include "j1App.h"
#include "p2Defs.h"
#include "j1CollisionManager.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "j1EntityManager.h"
#include "Player.h"

Entity::Entity() :
	sprite(nullptr),
	currentAnim(nullptr),
	actionState(IDLE),
	currentDir(D_DOWN),
	lastPos(iPoint()),
	currentPos(lastPos),
	col(nullptr),
	colPivot(iPoint()),
	type(ENTITY_TYPE(0)),
	life(1),
	damage(0),
	damaged(false),
	toDelete(false)
{
	anim.clear();
}

Entity::Entity(ENTITY_TYPE type) :
	sprite(nullptr),
	currentAnim(nullptr),
	actionState(IDLE),
	currentDir(D_DOWN),
	lastPos(iPoint()),
	currentPos(lastPos),
	col(nullptr),
	colPivot(iPoint()),
	type(type),
	life(1),
	damaged(false),
	toDelete(false)
{
	anim.clear();
}

Entity::~Entity()
{
	RELEASE(sprite);
	col->to_delete = true;
	col->parent = nullptr;
	anim.clear();
}

bool Entity::Draw()
{
	bool ret = true;

	currentAnim = &anim.find({ actionState, currentDir })->second;
	sprite->updateSprite(currentPos, currentAnim->pivot, currentAnim->getCurrentFrame(), currentAnim->flip);
	App->render->Draw(sprite);


	App->map->DebugPath(path);

	return ret;
}

bool Entity::LoadAttributes(pugi::xml_node attributes)
{
	bool ret = true;

	// base stats
	pugi::xml_node node = attributes.child("base");
	life = node.attribute("life").as_int(1);
	speed = node.attribute("speed").as_int(1);
	damage = node.attribute("damage").as_int(0);
	

	//collider
	node = attributes.child("collider");
	colPivot = { node.attribute("x").as_int(8), node.attribute("y").as_int(12) };
	col = App->collisions->AddCollider({ currentPos.x, currentPos.y, node.attribute("w").as_int(16), node.attribute("h").as_int(15) }, COLLIDER_TYPE(node.attribute("type").as_int(-1)), ((j1Module*)App->game));
	col->parent = this;
	node = attributes.child("animation");

	if (ret = LoadAnimations(node.attribute("file").as_string()))
	{
		node = attributes.child("texture");
		std::string texFile = node.attribute("file").as_string();

		SDL_Texture* tex = App->tex->Load(texFile.c_str());

		if (ret = (tex != NULL))
		{
			currentAnim = &anim.find({ actionState, currentDir })->second;
			sprite = new Sprite(tex, currentPos, SCENE, currentAnim->getCurrentFrame(), currentAnim->pivot);
		}
	}

	return ret;
}

bool Entity::LoadAnimations(std::string file)
{
	bool ret = true;

	pugi::xml_document	anim_file;
	pugi::xml_node		ent;
	char* buff;
	int size = App->fs->Load(file.c_str(), &buff);
	pugi::xml_parse_result result = anim_file.load_buffer(buff, size);
	RELEASE_ARRAY(buff);

	if (ret = (result != NULL))
	{
		ent = anim_file.child("animations");

		for (pugi::xml_node action = ent.first_child(); action != NULL; action = action.next_sibling())
		{
			for (pugi::xml_node dir = action.first_child().next_sibling(); dir != NULL; dir = dir.next_sibling())
			{
				std::pair<ACTION_STATE, DIRECTION> p;
				p.first = ACTION_STATE(action.child("name").attribute("value").as_int());
				p.second = DIRECTION(dir.first_child().attribute("name").as_int());

				Animation anims;
				anims.setAnimation(
					dir.first_child().attribute("x").as_int(),
					dir.first_child().attribute("y").as_int(),
					dir.first_child().attribute("w").as_int(),
					dir.first_child().attribute("h").as_int(),
					dir.first_child().attribute("frameNumber").as_int(),
					dir.first_child().attribute("margin").as_int());

				anims.loop = action.child("loop").attribute("value").as_bool();
				anims.speed = action.child("speed").attribute("value").as_float();
				anims.pivot.x = dir.first_child().attribute("pivot_x").as_int();
				anims.pivot.y = dir.first_child().attribute("pivot_y").as_int();
				anims.flip = SDL_RendererFlip(dir.first_child().attribute("flip").as_int());

				anim.insert(std::pair<std::pair<ACTION_STATE, DIRECTION>, Animation >(p, anims));
			}
		}
	}
	else
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
	}

	return ret;
}

//Displace the entity a given X and Y taking in account collisions w/map.
bool Entity::Move(int x, int y)
{
	bool ret = true;
	

	if (type == ENEMY)
	{
		currentPos.x += x;
		UpdateCollider();

		if (col->CheckEnemyMapCollision() != CZ_NONE)
		{
			currentPos.x -= x;
			ret = false;
		}

		currentPos.y += y;
		UpdateCollider();
		if (col->CheckEnemyMapCollision() != CZ_NONE)
		{
			currentPos.y -= y;
			ret = false;
		}
	}
	
	else
	{
		currentPos.x += x;
		UpdateCollider();

		//Jump right
		if (col->CheckPlayerMapJump() == CZ_RIGHT)
		{
			iPoint tmpPos = App->map->WorldToMap(currentPos.x, currentPos.y);
			for (int i = 1; i < 100; i++)
			{
				tmpPos.x += i;
				if (App->pathfinding->IsPlayerWalkable(tmpPos))
				{
					tmpPos.x++;
					App->game->em->player->toJump = tmpPos;
					break;
				}
			}
		}
		else //Jump left
			if (col->CheckPlayerMapJump() == CZ_LEFT)
			{
				iPoint tmpPos = App->map->WorldToMap(currentPos.x, currentPos.y);
				for (int i = 1; i < 100; i++)
				{
					tmpPos.x -= i;
					if (App->pathfinding->IsPlayerWalkable(tmpPos))
					{
						tmpPos.x--;
						App->game->em->player->toJump = tmpPos;
						break;
					}
				}
			}			


		//Check horizontal collisions
		if (col->CheckPlayerMapCollision() != CZ_NONE)
		{
			currentPos.x -= x;
			ret = false;
		}


		currentPos.y += y;
		UpdateCollider();

		//Jump down
		if (col->CheckPlayerMapJump() == CZ_DOWN)
		{
			iPoint tmpPos = App->map->WorldToMap(currentPos.x, currentPos.y);
			for (int i = 1; i < 100; i++)
			{
				tmpPos.y += i;
				if (App->pathfinding->IsPlayerWalkable(tmpPos))
				{
					tmpPos.y++;
					App->game->em->player->toJump = tmpPos;
					break;
				}
			}
		}

		//Check vertical collisions
		if (col->CheckPlayerMapCollision() != CZ_NONE)
		{
			currentPos.y -= y;
			ret = false;
		}
	}

	return ret;
}

bool Entity::MoveTo(int x, int y)
{
	bool ret = true;

	// check if position is available/walkable

	currentPos.x = x;
	currentPos.y = y;
	UpdateCollider();

	return ret;
}

void Entity::UpdateCollider()
{
	col->rect.x = currentPos.x - colPivot.x;
	col->rect.y = currentPos.y - colPivot.y;
}


//Use pathfinding to go to a given tile
bool Entity::GoTo(iPoint dest, int speed, float dt)
{

	dest = App->map->MapToWorld(dest.x, dest.y);

	iPoint movement;
	movement.SetToZero();

	if (dest.x > currentPos.x)
	{
		movement.x = 1;
		currentDir = D_RIGHT;
	}
	else if (dest.x < currentPos.x)
	{
		movement.x = -1;
		currentDir = D_LEFT;
	}

	if (dest.y > currentPos.y)
	{
		movement.y = 1;
		currentDir = D_DOWN;
	}

	else if (dest.y < currentPos.y)
	{
		movement.y = -1;
		currentDir = D_UP;
	}

	if (movement.IsZero())
		return false;

	Move(SDL_ceil(speed*dt)*movement.x, SDL_ceil(speed*dt)*movement.y);	
	return true;
}


//Makes entity look to player. Returns true if the direction changes
bool Entity::LookToPlayer()
{
	iPoint playerPos = App->game->em->player->currentPos;

	DIRECTION prevDir = currentDir;

	if (abs(playerPos.x - currentPos.x) < abs(playerPos.y - currentPos.y))
	{
		if (playerPos.y > currentPos.y)
			currentDir = D_DOWN;
		else currentDir = D_UP;
	}
	else
	{
		if (playerPos.x < currentPos.x)
			currentDir = D_LEFT;
		else currentDir = D_RIGHT;
	}

	if (prevDir == currentDir)
		return false;
	else return true;

}