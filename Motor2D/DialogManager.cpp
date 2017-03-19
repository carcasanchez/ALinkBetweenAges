#include "DialogManager.h"
#include "j1Input.h"
#include "j1Render.h"


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
	Load("dialogs.xml");
	dialogNode = dialogDataFile.child("npcs");
	return ret;
}

bool DialogManager::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
		Draw(1);

	return true;
}

bool DialogManager::Load(const char * file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.c_str(), file_name);

	char* buf;
	int size = App->fs->Load(tmp.GetString(), &buf);
	pugi::xml_parse_result result = dialogDataFile.load_buffer(buf, size);

	RELEASE(buf);

	if (result == NULL)
	{
		LOG("Could not load gui xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}
	return ret;
}

bool DialogManager::Draw(const int id)
{
	pugi::xml_node npc = dialogNode.child("npc");
	for (; npc != NULL; npc = npc.next_sibling())
	{
		if (npc.attribute("id").as_int() == id)
		{
			std::string text = npc.child("dialogue").child("text").first_child().value();
			if (npc.child("options").attribute("interaction").as_bool() == true)
			{
				//Choose option with UI method. TODO
			}
		}
	}
	
	return false;
}

