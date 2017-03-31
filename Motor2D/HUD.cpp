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
	
	LoadPause(pause_folder);
	LoadHud(hud_folder);


	return false;
}

bool Hud::Start()
{
	App->inputM->AddListener(this);
	return true;
}

bool Hud::Update(float dt)
{
	
	if (pause_transition == PAUSE_UP)
		PauseOut(dt);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		App->game->pause = !(App->game->pause);
		IntoPause();
		pause_transition = PAUSE_DOWN;
	}

	return false;
}

void Hud::OnInputCallback(INPUTEVENT new_event, EVENTSTATE state)
{
	
	switch (new_event)
	{
	case PAUSE:
		if (state == E_DOWN)
		{
			if (App->game->pause)
			{
				pause_transition = PAUSE_UP;
				main_menu->SetAnimationTransition(T_FLY_UP, 1500, { main_menu->Interactive_box.x, -650 });
			}
			else
			{
				App->game->pause = true;
				IntoPause();
				main_menu->SetAnimationTransition(T_MOVE_DOWN, 2000, { main_menu->Interactive_box.x, 140 });
				App->inputM->SetGameContext(IN_MENU);
				pause_transition = PAUSE_DOWN;
			}	
		}
		break;

	case UP:
		for (std::vector<UI_Image*>::reverse_iterator it = pause_selectables.rbegin(); it != pause_selectables.rend(); it++)
		{
			
			if ((*it)->active && (it + 1) != pause_selectables.rend())
			{
				(*it)->active = false;
				(*(it + 1))->Set_Active_state(true);
				break;
			}
				
		}
		break;

	case DOWN:
		for (std::vector<UI_Image*>::iterator it = pause_selectables.begin(); it != pause_selectables.end(); it++)
		{

			if ((*it)->active && (it + 1) != pause_selectables.end())
			{
				(*it)->active = false;
				(*(it + 1))->Set_Active_state(true);
				break;
			}
		}
		break;

	case CONFIRM:

		if (resume->active)
		{
			pause_transition = PAUSE_UP;
			main_menu->SetAnimationTransition(T_FLY_UP, 1500, { main_menu->Interactive_box.x, -650 });
		}

		if (quit->active)
		{
			App->game->quit_game = true;
		}

		break;

	case DECLINE:
		pause_transition = PAUSE_UP;
		main_menu->SetAnimationTransition(T_FLY_UP, 1500, { main_menu->Interactive_box.x, -650 });
		break;

	}

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

		//selecable items go into vector
		resume->Set_Image_Texture(LoadRect(pause_node.child("selector")));
		quit->Set_Image_Texture(LoadRect(pause_node.child("selector")));

		pause_selectables.push_back(resume);
		pause_selectables.push_back(quit);

		SetPauseElements();

		App->gui->CreateScreen(main_menu);
		main_menu->AddChild(item_menu);
		main_menu->AddChild(resume);
		main_menu->AddChild(quit);
	}

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

void Hud::SetPauseElements()
{
	main_menu->Set_Interactive_Box({ 50, -650,0,0 });
	item_menu->Set_Interactive_Box({720,0,0,0});
	resume->Set_Interactive_Box({ 72,86,0,0 });
	quit->Set_Interactive_Box({ 72,528,0,0 });

	
	quit->Set_Active_state(false);

	main_menu->Set_Active_state(true);
}

bool Hud::IntoPause()
{
	main_menu->Set_Active_state(true);
	resume->Set_Active_state(true);

	quit->active = false;

	return true;
}

void Hud::GonePause()
{
	main_menu->QuitFromRender();
	
	for (std::vector<UI_Image*>::iterator it = pause_selectables.begin(); it != pause_selectables.end(); it++)
		(*it)->Set_Active_state(false);

	App->inputM->SetGameContext(IN_GAME);
}


void Hud::PauseOut(float dt)
{
	if (main_menu->GetCurrentTransition() == NO_AT)
	{
		App->game->pause = false;
		GonePause();
		pause_transition = PAUSE_NO_MOVE;
	}
}

bool Hud::LoadHud(string file)
{
	bool ret = true;

	// load xml attributes
	pugi::xml_document	hud_file;
	char* buff;
	int size = App->fs->Load(file.c_str(), &buff);
	pugi::xml_parse_result result = hud_file.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		Rupees = (UI_Image*)App->gui->Add_element(IMAGE, App->game);
		Bombs = (UI_Image*)App->gui->Add_element(IMAGE, App->game);
		Arrows = (UI_Image*)App->gui->Add_element(IMAGE, App->game);
		items_frame = (UI_Image*)App->gui->Add_element(IMAGE, App->game);
		life = (UI_Image*)App->gui->Add_element(IMAGE, App->game);
		stamina_bar = (UI_Image*)App->gui->Add_element(IMAGE, App->game);

		pugi::xml_node hud_node = hud_file.child("images");
		
		Rupees->Set_Image_Texture(LoadRect(hud_node.child("little_items").child("rupees")));
		Bombs->Set_Image_Texture(LoadRect(hud_node.child("little_items").child("bombs")));
		Arrows->Set_Image_Texture(LoadRect(hud_node.child("little_items").child("arrows")));

		items_frame->Set_Image_Texture(LoadRect(hud_node.child("items").child("frame")));

		life->Set_Image_Texture(LoadRect(hud_node.child("life")));

		stamina_bar->Set_Image_Texture(LoadRect(hud_node.child("stamina").child("bar")));
	
		SetHudElements();


		hud_screen->AddChild(Rupees);
		hud_screen->AddChild(Bombs);
		hud_screen->AddChild(Arrows);
		hud_screen->AddChild(items_frame);
		hud_screen->AddChild(life);
		hud_screen->AddChild(stamina_bar);

	}



	return false;
}

void Hud::SetHudElements()
{
	Rupees->Set_Interactive_Box({ 175,20,0,0 });
	Bombs->Set_Interactive_Box({ 250,20,0,0 });
	Arrows->Set_Interactive_Box({ 325,20,0,0 });

	items_frame->Set_Interactive_Box({ 50, 20,0,0 });

	life->Set_Interactive_Box({ 700, 20,0,0 });

	stamina_bar->Set_Interactive_Box({ 700, 50,0,0 });



}
