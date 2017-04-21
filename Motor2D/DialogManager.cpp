#include "DialogManager.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Console.h"
#include "j1App.h"



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
	pugi::xml_parse_result result = dialogueDataFile.load_buffer(buf, size);

	RELEASE_ARRAY(buf);

	if (result == NULL)
	{
		LOG("Could not load gui xml file %s. pugi error: %s", dialogueDataFile, result.description());
		ret = false;
	}

	return ret;
}

bool DialogManager::Start()
{
	bool ret = true;
	dialogueNode = dialogueDataFile.child("npcs");
	// Allocate memory
	int i = 0;
	for (pugi::xml_node npc = dialogueNode.child("npc"); npc != NULL; npc = npc.next_sibling(), i++)
	{
		//Allocate Dialog with his ID and State
		Dialog* tmp = new Dialog(npc.attribute("id").as_int());
		dialog.push_back(tmp);

		//Allocate text
		for (pugi::xml_node dialogue = npc.child("dialogue"); dialogue != NULL; dialogue = dialogue.next_sibling())
		{
			for (pugi::xml_node text = dialogue.child("text"); text != NULL; text = text.next_sibling("text"))
			{
				TextLine* tmp = new TextLine(dialogue.attribute("state").as_int(), text.attribute("value").as_string());
				dialog[i]->texts.push_back(tmp);
			}
		}
	}

	//Prepare UI to print
	screen = App->gui->CreateScreen(screen);
	text_on_screen = (UI_String*)App->gui->Add_element(STRING, this);
	text_on_screen->Set_Active_state(false);
	text_on_screen->Set_Interactive_Box({ 150, 600, 400, 20 });
	text_on_screen->SetBlitTimeMS(1500);

	screen->AddChild(text_on_screen);

	return ret;
}

bool DialogManager::BlitDialog(int id, int state)
{
	//Find the correct ID
	for (int i = 0; i < dialog.size(); i++)
	{
		if (dialog[i]->id == id)
		{
			int vectorPos; //Looks the exact vector position deppending of the state
			for(vectorPos = 0; dialog[i]->texts[vectorPos]->state != state; vectorPos++)
			{}
			if ((dialogueStep+vectorPos) >= dialog[i]->texts.size())
			{
				return false;
			}
			if (dialog[i]->texts[dialogueStep + vectorPos]->state == state)
			{
				text_on_screen->Set_String((char*)dialog[i]->texts[dialogueStep + vectorPos]->line->c_str());
				return true;
			}
		}
	}

	return false;
}

DialogManager::~DialogManager()
{
	dialog.clear();
}

Dialog::Dialog(int id) : id(id)
{}

Dialog::~Dialog()
{
	texts.clear();
}

TextLine::TextLine(int NPCstate, std::string text) : state(NPCstate)
{
	line = new std::string(text);
}

TextLine::~TextLine()
{
	delete line;
}

