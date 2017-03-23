#include "DialogManager.h"
#include "j1Input.h"
#include "j1Render.h"


DialogManager::DialogManager() : j1Module()
{
	name = ("dialogue");
}

bool DialogManager::Awake(pugi::xml_node & config)
{
	bool ret = true;

	LOG("Loading DialogManager data");
	folder = config.child("dialogues").attribute("folder").as_string();
	path = config.child("dialogues").first_child().attribute("file").as_string();

	std::string tmp = folder + path;

	char* buf;
	int size = App->fs->Load(tmp.c_str(), &buf);
	pugi::xml_parse_result result = dialogDataFile.load_buffer(buf, size);

	RELEASE(buf);

	if (result == NULL)
	{
		LOG("Could not load gui xml file %s. pugi error: %s", dialogDataFile, result.description());
		ret = false;
	}


	return ret;
}

bool DialogManager::Start()
{
	bool ret = true;
	dialogNode = dialogDataFile.child("npcs");
	// Reseve memory to dialog
	int i = 0;
	for (pugi::xml_node npc = dialogNode.child("npc"); npc != NULL; npc = npc.next_sibling(), i++)
	{
		Dialog* tmp = new Dialog(npc.attribute("id").as_int(), npc.child("dialogue").attribute("state").as_uint());
		dialog.push_back(tmp);

		for (npc = npc.child("dialogue"); npc != NULL; npc = npc.next_sibling())
		{
			int j = 0;
			pugi::xml_node textx = npc.child("text");
			for (pugi::xml_node text = npc.child("text"); text != NULL; text = text.next_sibling())
			{
				Line* tmp = new Line(false, text.attribute("value").as_string());
				dialog[i]->texts.push_back(tmp);
			}
			for (pugi::xml_node option = npc.child("options").child("option"); option != NULL; option = option.next_sibling())
			{
				Line* tmp = new Line(true, option.attribute("value").as_string());
				dialog[i]->texts.push_back(tmp);
			}
			for (pugi::xml_node response = npc.child("response"); response != NULL; response = response.next_sibling())
			{
				Line* tmp = new Line(false, response.attribute("value").as_string());
				dialog[i]->texts.push_back(tmp);
			}
		}
	}
	return ret;
}

bool DialogManager::Update(float dt)
{

	return true;
}

bool DialogManager::DialogCharge(const int id)
{
	return true;
}

Dialog::Dialog(int id, int state) : id(id), state(state)
{}

Dialog::Dialog()
{}

Dialog::~Dialog()
{
	texts.clear();
}

Line::Line(bool interaction, std::string text) : interaction(interaction)
{
	line = new std::string(text);
}

Line::~Line()
{}

