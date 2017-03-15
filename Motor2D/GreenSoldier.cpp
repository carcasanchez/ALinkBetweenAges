#include "j1App.h"
#include "p2Log.h"
#include "GreenSoldier.h"


bool GreenSoldier::Spawn(std::string file, iPoint pos)
{
	bool ret = true;

	//set position
	currentPos = lastPos = pos;

	// load xml attributes
	pugi::xml_document	attributesFile;
	char* buff;
	int size = App->fs->Load(file.c_str(), &buff);
	pugi::xml_parse_result result = attributesFile.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}

	else
	{
		pugi::xml_node attributes = attributesFile.child("attributes").child("green_soldier");

		//LoadAttributes(attributes);
	}
	return ret;
}

bool GreenSoldier::Update(float dt)
{
	return true;
}

