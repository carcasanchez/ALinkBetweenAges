#include "Object.h"
#include "j1GameLayer.h"
#include "j1CollisionManager.h"
#include "j1EntityManager.h"
#include "Player.h"

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
		}		
		LoadAttributes(attributes);

		switch (type)
		{
		case LINK_ARROW:
			if (currentDir == D_RIGHT || currentDir == D_LEFT)
			{
				col->rect.h = 7;
				col->rect.w = 15;
			}
			break;
		}

		
	}

	return ret;
}

void Object::OnDeath()
{
	toDelete = true;
}
