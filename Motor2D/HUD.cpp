#include "HUD.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1FileSystem.h"
#include "j1GameLayer.h"
#include "p2Log.h"

Hud::Hud()
{
}

bool Hud::Awake(pugi::xml_node& conf)
{
	pugi::xml_node hud_attributes = conf.child("hud");

	string hud_folder = hud_attributes.attribute("folder").as_string();
	string pause_folder = hud_attributes.attribute("folder").as_string();
	
	hud_folder += hud_attributes.child("ingame").attribute("file").as_string();
	pause_folder += hud_attributes.child("pause").attribute("file").as_string();

	hud_screen = App->gui->CreateScreen(hud_screen);
	pause_screen = App->gui->CreateScreen(pause_screen);

	LoadPause(pause_folder);

	return false;
}

bool Hud::LoadPause(string file)
{
	bool ret = true;

	// load xml attributes
	pugi::xml_document	pause_file;
	char* buff;
	int size = App->fs->Load(file.c_str(), &buff);
	pugi::xml_parse_result result = pause_file.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		//Initialize images
		main_menu = (UI_Image*)App->gui->Add_element(IMAGE, App->game);
		item_menu = (UI_Image*)App->gui->Add_element(IMAGE, App->game);
		resume = (UI_Image*)App->gui->Add_element(IMAGE, App->game);
		quit = (UI_Image*)App->gui->Add_element(IMAGE, App->game);

		pugi::xml_node pause_node = pause_file.child("images");

		main_menu->Set_Image_Texture(LoadRect(pause_node.child("main")));
		item_menu->Set_Image_Texture(LoadRect(pause_node.child("item")));
		resume->Set_Image_Texture(LoadRect(pause_node.child("selector")));
		quit->Set_Image_Texture(LoadRect(pause_node.child("selector")));
	}

	pause_screen->AddChild(main_menu);
	pause_screen->AddChild(item_menu);
	pause_screen->AddChild(resume);
	pause_screen->AddChild(quit);


	return ret;
}

SDL_Rect Hud::LoadRect(pugi::xml_node node)
{
	SDL_Rect ret;

	int x = node.attribute("x").as_int();
	int y = node.attribute("y").as_int();
	int w = node.attribute("w").as_int();
	int h = node.attribute("h").as_int();

	ret = { x,y,w,h };

	return ret;
}
