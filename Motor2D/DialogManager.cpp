#include "DialogManager.h"

DialogManager::DialogManager()
{

}

bool DialogManager::Awake(pugi::xml_node & config)
{
	LOG("Loading DialogManager data");
	path = config.child("data").attribute("file").as_string();
	return true;
} 

bool DialogManager::Start()
{
	bool ret = true;
	pugi::xml_document	dialogDataFile;
	char* buff;
	int size = App->fs->Load(path.c_str(), &buff);
	pugi::xml_parse_result result = dialogDataFile.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load DialogManager xml file. Pugi error: %s", result.description());
		ret = false;
	}

	return ret;
}


