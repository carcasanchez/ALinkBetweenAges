#include "Object.h"
#include "j1GameLayer.h"
#include "j1CollisionManager.h"
#include "Player.h"
#include "j1Map.h"

bool Object::Spawn(std::string file, iPoint pos, OBJECT_TYPE type)
{
	bool ret = true;

	// set position
	currentPos = lastPos = pos;
	this->objectType = type;

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
		attributes = attributesFile.child("attributes");
		switch (type)
		{
			case BOOK:
				attributes = attributes.child("book");
				break;
			case LINK_ARROW:
				attributes = attributes.child("arrow");
				damage = 1;
				currentDir = App->game->em->player->currentDir;
				break;
			case GREEN_RUPEE:
				attributes = attributes.child("green_rupee");
				((Rupee*)this)->rupeeValue = 1;
				break;
			case BLUE_RUPEE:
				attributes = attributes.child("blue_rupee");
				((Rupee*)this)->rupeeValue = 5;
				break;
			case RED_RUPEE:
				attributes = attributes.child("red_rupee");
				((Rupee*)this)->rupeeValue = 20;
				break;
			case LIFEHEART:
				attributes = attributes.child("heart");
				break;
			case BUSH:
				attributes = attributes.child("bush");
				break;
			case POT:
				attributes = attributes.child("pot");
				break;
			case OCTO_STONE:
				attributes = attributes.child("octo_stone");
				break;
			case CHEST:
				actionState = CLOSE;
				attributes = attributes.child("chest");
				break;
			case MAGIC_SLASH:
				attributes = attributes.child("magic_slash");
				break;
			case BOMB:
				attributes = attributes.child("bomb");
				break;
			case BOMB_EXPLOSION:
				attributes = attributes.child("bomb_explosion");
				break;
			case HEART_CONTAINER:
				attributes = attributes.child("heart_container");
				break;
			case ARROW_DROP:
				attributes = attributes.child("arrow_drop");
				break;
			case BOMB_DROP:
				attributes = attributes.child("bomb_drop");
				break;
			case LIFE_POTION:
				attributes = attributes.child("life_potion");
				break;
			case BOMB_SAC:
				attributes = attributes.child("bomb_sac");
				break;
			case ZELDA_ARROW:
				attributes = attributes.child("zelda_arrow");
				break;
			case SWORD_BOLT:
				attributes = attributes.child("sword_bolt");
				break;
			case FALLING_BOLT:
				attributes = attributes.child("falling_bolt");
				break;
			case BOW:
				attributes = attributes.child("bow");
				break;
			case STAMINA_POTION:
				attributes = attributes.child("stamina_potion");
				break;
			case PILLAR:
				attributes = attributes.child("pillar");
				break;
			case STONE:
				attributes = attributes.child("stone");
				break;
			case BOSS_KEY:
				attributes = attributes.child("boss_key");
				break;
		}		

		LoadAttributes(attributes);

		price = attributes.child("base").attribute("price").as_int();

		switch (type)
		{
		case LINK_ARROW:
			if (currentDir == D_RIGHT || currentDir == D_LEFT)
			{
				col->rect.h = 7;
				col->rect.w = 15;
			}
			break;

		case ARROW_DROP:
			currentDir = D_UP;
			break;

		case ZELDA_ARROW:
			currentDir = D_DOWN;
			break;
		
		}

		
	}

	return ret;
}

void Object::OnDeath()
{
	toDelete = true;
}

bool Bomb::ExplodeBomb()
{
		App->game->em->CreateObject(1, (this->currentPos.x / (App->map->data->tile_width)), (this->currentPos.y) / (App->map->data->tile_height), BOMB_EXPLOSION);
		this->life = -1;

		return true;
}

bool BombExplosion::DeleteExplode()
{
	if (dead_counter.ReadMs() > dead_time)
	{
		this->life = -1;
		return true;
	}
	else
		return false;
}

bool FallingBolt::Update(float dt)
{
	iPoint playerPos = App->map->WorldToMap(App->game->em->player->currentPos.x, App->game->em->player->currentPos.y);
	GoTo(playerPos, speed, dt);
	currentDir = D_DOWN;
	return false;
}

bool Pillar::Update(float dt)
{
	if (App->render->InsideCameraZone(col->rect))
		col->active = true;
	else col->active = false;

	return true;
}