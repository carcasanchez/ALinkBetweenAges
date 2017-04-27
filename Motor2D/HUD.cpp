#include "HUD.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1FileSystem.h"
#include "j1GameLayer.h"
#include "j1EntityManager.h"
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

	LookLife();

	rupees_counter->LookNumber(App->game->em->player->rupees);
	bombs_counter->LookNumber(App->game->em->player->bombs);
	arrows_counter->LookNumber(App->game->em->player->arrows);


	return false;
}

bool Hud::CleanUp()
{
	RELEASE(numbers);

	
	//RELEASE(empty_heart);
	//delete medium_heart;
	//delete full_heart;

	RELEASE(stamina_container);
	RELEASE(stamina_green);
	RELEASE(stamina_yellow);
	RELEASE(stamina_red);

	return true;
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
				if (pause_transition == PAUSE_NO_MOVE)
				{
					pause_transition = PAUSE_UP;
					main_menu->SetAnimationTransition(T_FLY_UP, 1000, { main_menu->Interactive_box.x, -650 });
				}
			}
			else
			{
				IntoPause();
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
			if (pause_transition == PAUSE_NO_MOVE)
			{
				pause_transition = PAUSE_UP;
				main_menu->SetAnimationTransition(T_FLY_UP, 1000, { main_menu->Interactive_box.x, -650 });
			}
		}

		if (quit->active)
		{
			App->game->quit_game = true;
		}

		break;

	case DECLINE:
		if (pause_transition == PAUSE_NO_MOVE)
		{
			pause_transition = PAUSE_UP;
			main_menu->SetAnimationTransition(T_FLY_UP, 1000, { main_menu->Interactive_box.x, -650 });
		}
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
	RELEASE_ARRAY(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		//Initialize images
		pugi::xml_node pause_node = pause_file.child("images");

		main_menu = (UI_Image*)LoadUIElement(pause_node.child("main"), nullptr, IMAGE);
		item_menu = (UI_Image*)LoadUIElement(pause_node.child("item"), main_menu, IMAGE);
		resume = (UI_Image*)LoadUIElement(pause_node.child("resume"), main_menu, IMAGE);
		quit = (UI_Image*)LoadUIElement(pause_node.child("quit"), main_menu, IMAGE);

		//selecable items go into vector
		pause_selectables.push_back(resume);
		pause_selectables.push_back(quit);

		App->gui->CreateScreen(main_menu);

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

UI_element* Hud::LoadUIElement(pugi::xml_node node, UI_element* screen, UI_TYPE type)
{
	UI_element* ret = App->gui->Add_element(type, App->game);

	if (ret)
	{
		if (type == IMAGE)
		{
			UI_Image* tmp = (UI_Image*)ret;

			tmp->Set_Image_Texture(LoadRect(node));
			tmp->Set_Interactive_Box({ node.attribute("pos_x").as_int(), node.attribute("pos_y").as_int(), 0, 0 });
			tmp->Set_Active_state(node.attribute("active").as_bool());

			if (screen)
				screen->AddChild(ret);

			return ret;
		}

		if (type == COUNTER)
		{
			UI_Counter* tmp = (UI_Counter*)ret;

			tmp->SetImage(numbers, 14, 14);
			tmp->Set_Interactive_Box({ node.attribute("pos_x").as_int(), node.attribute("pos_y").as_int(), 0, 0 });
			tmp->Set_Active_state(node.attribute("active").as_bool());

			if (screen)
				screen->AddChild(ret);

			return ret;

		}

		if (type == STAMINA_BAR)
		{
			UI_Stamina* tmp = (UI_Stamina*)ret;

			tmp->Set_Interactive_Box({ node.attribute("pos_x").as_int(), node.attribute("pos_y").as_int(), 0, 0 });
			tmp->SetDrawRect({ node.attribute("draw_x").as_int() ,  node.attribute("draw_x").as_int() ,0,0 });
			tmp->SetBackground(stamina_container);
			tmp->SetStamina(stamina_green);

			if (screen)
				screen->AddChild(ret);

			return ret;

		}

	}


}


bool Hud::IntoPause()
{
	App->game->pause = true;
	main_menu->SetAnimationTransition(T_MOVE_DOWN, 1000, { main_menu->Interactive_box.x, 140 });
	App->inputM->SetGameContext(IN_MENU);

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
	RELEASE_ARRAY(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}
	else
	{
	
		pugi::xml_node hud_node = hud_file.child("images");

		//Numbers counter image
		numbers = (UI_Image*)LoadUIElement(hud_node.child("numbers"), nullptr, IMAGE);
		
		//little items
		Rupees = (UI_Image*)LoadUIElement(hud_node.child("little_items").child("rupees"), hud_screen, IMAGE);
		rupees_counter = (UI_Counter*)LoadUIElement(hud_node.child("counters").child("rupees"), Rupees, COUNTER);

		Bombs = (UI_Image*)LoadUIElement(hud_node.child("little_items").child("bombs"), hud_screen, IMAGE);
		bombs_counter = (UI_Counter*)LoadUIElement(hud_node.child("counters").child("bombs"), Bombs, COUNTER);

		Arrows = (UI_Image*)LoadUIElement(hud_node.child("little_items").child("arrows"), hud_screen, IMAGE);
		arrows_counter = (UI_Counter*)LoadUIElement(hud_node.child("counters").child("arrows"), Arrows, COUNTER);

		//items
		items_frame = (UI_Image*)LoadUIElement(hud_node.child("items").child("frame"), hud_screen, IMAGE);

		//life
		life = (UI_Image*)LoadUIElement(hud_node.child("life"), hud_screen, IMAGE);
		empty_heart = (UI_Image*)LoadUIElement(hud_node.child("heart").child("empty"), hud_screen, IMAGE);
		medium_heart = (UI_Image*)LoadUIElement(hud_node.child("heart").child("mid"), hud_screen, IMAGE);
		full_heart = (UI_Image*)LoadUIElement(hud_node.child("heart").child("full"), hud_screen, IMAGE);
 
		//stamina
		stamina_container = (UI_Image*)LoadUIElement(hud_node.child("stamina").child("container"), nullptr, IMAGE);
		stamina_green = (UI_Image*)LoadUIElement(hud_node.child("stamina").child("sta_g"), nullptr, IMAGE);
		stamina_yellow = (UI_Image*)LoadUIElement(hud_node.child("stamina").child("sta_y"), nullptr, IMAGE);
		stamina_red = (UI_Image*)LoadUIElement(hud_node.child("stamina").child("sta_r"), nullptr, IMAGE);
		stamina_bar = (UI_Stamina*)LoadUIElement(hud_node.child("stamina").child("bar"), hud_screen, STAMINA_BAR);

		LoadHearts();

	}

	return false;
}



void Hud::LoadHearts()
{
	for (int i = 0; i < 10; i++)
	{
		UI_Heart* new_heart = (UI_Heart*)App->gui->Add_element(HEART, App->game);

		new_heart->heart_img = full_heart;
		new_heart->Set_Interactive_Box({ 700 + (i * ((full_heart->Image.w * 2) + space_between_hearts)), 55, 0,0 });
		new_heart->Set_Active_state(false);

		hud_screen->AddChild(new_heart);
		hearts.push_back(new_heart);
	}
	
}

void Hud::LookLife()
{
	LookNumHearts();

	int full_hearts = 1;
	for (vector<UI_Heart*>::iterator it = hearts.begin(); it != hearts.end(); it++)
	{
		if ((*it)->active)
		{
			if (full_hearts <= App->game->em->player->life)
			{
				(*it)->ChangeState(HEARTSTATE::FULL_HEART);
				full_hearts++;
			}
			else (*it)->ChangeState(HEARTSTATE::EMPTY_HEART);
		}
		else break;
	}

}

void Hud::LookNumHearts()
{
	if (active_hearts < App->game->em->player->maxLife)
	{
		for (vector<UI_Heart*>::iterator it = hearts.begin(); it != hearts.end() && active_hearts < App->game->em->player->maxLife; it++)
		{
			if ((*it)->active)
				continue;

			(*it)->Set_Active_state(true);
			active_hearts++;
		}
	}
}

