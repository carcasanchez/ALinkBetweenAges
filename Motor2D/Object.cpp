#include "Object.h"
#include "j1CollisionManager.h"

bool Object::Spawn(std::string file, iPoint pos)
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
		pugi::xml_node attributes = attributesFile.child("attributes");
		LoadAttributes(attributes);
	}

	return ret;
}

void Object::OnDeath()
{
	toDelete = true;
}
