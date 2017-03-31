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
	toDelete(false),
	currentPatrolPoint(0)
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
	toDelete(false),
	currentPatrolPoint(0)
{
	anim.clear();
}

Entity::~Entity()
{
	RELEASE(sprite);
	RELEASE(currentAnim);

	col->to_delete = true;
	col = nullptr;

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

	//TODO: load three links animation properly
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
	RELEASE(buff);

	if (ret = (result != NULL))
	{
		ent = anim_file.child("animations");

		for (pugi::xml_node action = ent.child("IDLE"); action != NULL; action = action.next_sibling())
		{
			for (pugi::xml_node dir = action.child("UP"); dir != action.child("loop"); dir = dir.next_sibling())
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
	

	if (type == LINK)
	{
		currentPos.x += x;
		UpdateCollider();

		if (col->CheckPlayerMapCollision() != CZ_NONE)
		{
			currentPos.x -= x;
			ret = false;
		}

		currentPos.y += y;
		UpdateCollider();
		if (col->CheckPlayerMapCollision() != CZ_NONE)
		{
			currentPos.y -= y;
			ret = false;
		}
	}

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
	/*//Create path if player changes tile
	if (dest != currentDest)
	{
		currentDest = dest;
		iPoint origin = App->map->WorldToMap(currentPos.x, currentPos.y);
		if (App->pathfinding->CreatePath(origin, currentDest))
		{
			path = App->pathfinding->ReturnPath();
			path.pop_back();
			path.pop_back();
		}
	}


	if (path.size() != 0)
	{
		iPoint immediateDest = App->map->GetTileCenter((*path.end()));

		if (immediateDest.x > currentPos.x)
		{
			currentPos.x += SDL_ceil(speed * dt);
			currentDir = D_RIGHT;
		}
		else if (immediateDest.x < currentPos.x)
		{
			currentPos.x -= SDL_ceil(speed * dt);
			currentDir = D_LEFT;
		}

		if (immediateDest.y > currentPos.y)
		{
			currentPos.y += SDL_ceil(speed * dt);
			currentDir = D_DOWN;
		}

		else if (immediateDest.y < currentPos.y)
		{
			currentPos.y -= SDL_ceil(speed * dt);
			currentDir = D_UP;
		}


		if (abs(immediateDest.x - currentPos.x) < 2 && abs(immediateDest.y - currentPos.y) < 2)
		{
			path.pop_back();
		}

		return true;
	}
	*/

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