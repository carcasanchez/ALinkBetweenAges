#include "Object.h"
#include "j1CollisionManager.h"

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
			case ARROW:
				attributes = attributes.child("arrow");
				break;
		}		
		LoadAttributes(attributes);
	}

	return ret;
}

void Object::OnDeath()
{
	toDelete = true;
}
