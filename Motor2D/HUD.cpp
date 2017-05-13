#include "HUD.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1FileSystem.h"
#include "j1CutSceneManager.h"
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
	hud_screen->Set_Active_state(false);

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
	if (start_menu_screen->active == false && App->cutsceneM->CutsceneReproducing() == false)
	{
		if (pause_transition == PAUSE_UP)
			PauseOut(dt);

		LookLife();

		rupees_counter->LookNumber(App->game->em->player->rupees);
		bombs_counter->LookNumber(App->game->em->player->bombs);
		arrows_counter->LookNumber(App->game->em->player->arrows);

		LookInventory();
	}

	//Basura para que ricard deje de llorar

	if (App->inputM->GetGameContext() == GAMECONTEXT::IN_MENU)
	{
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			if (App->game->pause && pause_transition == PAUSE_NO_MOVE)
			{
				pause_transition = PAUSE_UP;
				main_menu->SetAnimationTransition(T_FLY_UP, 1000, { main_menu->Interactive_box.x, -650 });
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		{
			if (!start_menu_screen->active)
			{
				for (std::vector<UI_Image*>::reverse_iterator it = pause_selectables.rbegin(); it != pause_selectables.rend(); it++)
				{

					if ((*it)->active && (it + 1) != pause_selectables.rend())
					{
						(*it)->active = false;
						(*(it + 1))->Set_Active_state(true);
						break;
					}

				}
			}
			else
			{
				for (std::vector<UI_Image*>::reverse_iterator it = start_menu_selectables.rbegin(); it != start_menu_selectables.rend(); it++)
				{

					if ((*it)->active && (it + 1) != start_menu_selectables.rend())
					{
						(*it)->active = false;
						(*(it + 1))->Set_Active_state(true);
						break;
					}

				}
			}

		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			if (!start_menu_screen->active)
			{
				for (std::vector<UI_Image*>::iterator it = pause_selectables.begin(); it != pause_selectables.end(); it++)
				{
					if ((*it)->active && (it + 1) != pause_selectables.end())
					{
						(*it)->active = false;
						(*(it + 1))->Set_Active_state(true);
						break;
					}
				}
			}
			else
			{
				for (std::vector<UI_Image*>::iterator it = start_menu_selectables.begin(); it != start_menu_selectables.end(); it++)
				{
					if ((*it)->active && (it + 1) != start_menu_selectables.end())
					{
						(*it)->active = false;
						(*(it + 1))->Set_Active_state(true);
						break;
					}
				}
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			if (!start_menu_screen->active)
			{
				if (resume->active)
				{
					if (pause_transition == PAUSE_NO_MOVE)
					{
						pause_transition = PAUSE_UP;
						main_menu->SetAnimationTransition(T_FLY_UP, 1000, { main_menu->Interactive_box.x, -650 });
					}
				}

				if (save->active)
				{
					App->SaveGame("saves.xml");
					saved_game->Set_Active_state(true);
				}

				if (load->active)
				{
					App->LoadGame("saves.xml");
					loaded_game->Set_Active_state(true);
				}

				if (controls->active)
				{

				}

				if (quit->active)
				{
					App->game->quit_game = true;
					App->game->pause = false;
				}
			}
			else
			{
				if (start_continue->active)
				{
					//App->LoadGame("saves.xml");
					//App->inputM->SetGameContext(GAMECONTEXT::IN_MENU);
				}

				if (start_new_game->active)
				{
					StartGame();
					App->cutsceneM->StartCutscene(1);
				}

				if (start_quit->active)
				{
					App->game->quit_game = true;
					App->game->pause = false;
				}
			}
		}
	}

	if (App->inputM->GetGameContext() == GAMECONTEXT::IN_GAME)
	{
		if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
			IntoPause();
	}

	


	return false;
}

bool Hud::CleanUp()
{
		
	//RELEASE(empty_heart);
	//RELEASE (medium_heart);
	//delete full_heart;


	RELEASE(numbers);
	RELEASE(stamina_container);
	RELEASE(stamina_green);
	RELEASE(stamina_yellow);
	RELEASE(stamina_red);

	RELEASE(items_frame_inactive);
	RELEASE(items_frame_using);

	RELEASE(life_container);
	RELEASE(full_life);
	RELEASE(mid_life);
	RELEASE(low_life);

	return true;
}

void Hud::OnInputCallback(INPUTEVENT new_event, EVENTSTATE state)
{

	if (!start_menu_screen->active)
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

			if (state == E_DOWN)
			{
				for (std::vector<UI_Image*>::reverse_iterator it = pause_selectables.rbegin(); it != pause_selectables.rend(); it++)
				{

					if ((*it)->active && (it + 1) != pause_selectables.rend())
					{
						(*it)->active = false;
						(*(it + 1))->Set_Active_state(true);
						break;
					}

				}
			}
			break;

		case DOWN:
			if (state == E_DOWN)
			{
				for (std::vector<UI_Image*>::iterator it = pause_selectables.begin(); it != pause_selectables.end(); it++)
				{
					if ((*it)->active && (it + 1) != pause_selectables.end())
					{
						(*it)->active = false;
						(*(it + 1))->Set_Active_state(true);
						break;
					}
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

			if (save->active)
			{
				App->SaveGame("saves.xml");
				saved_game->Set_Active_state(true);
			}

			if (load->active)
			{
				App->LoadGame("saves.xml");
				loaded_game->Set_Active_state(true);
			}

			if (controls->active)
			{

			}

			if (quit->active)
			{
				App->game->quit_game = true;
				App->game->pause = false;
			}



			break;

		case DECLINE:
			if (pause_transition == PAUSE_NO_MOVE)
			{
				pause_transition = PAUSE_UP;
				main_menu->SetAnimationTransition(T_FLY_UP, 1000, { main_menu->Interactive_box.x, -650 });
			}
			break;

		case USE_ITEM:
		{
			if (!item_active)
			{
				item_frame->Image = items_frame_using->Image;
				item_active = true;
			}
			break;
		}

		case STOP_ITEM:
		{
			if (item_active)
			{
				item_frame->Image = items_frame_inactive->Image;
				item_active = false;
			}
			break;
		}

		}
	}

	else
	{

		switch (new_event)
		{
		case UP:

			if (state == E_DOWN)
			{
				for (std::vector<UI_Image*>::reverse_iterator it = start_menu_selectables.rbegin(); it != start_menu_selectables.rend(); it++)
				{

					if ((*it)->active && (it + 1) != start_menu_selectables.rend())
					{
						(*it)->active = false;
						(*(it + 1))->Set_Active_state(true);
						break;
					}

				}
			}
			break;

		case DOWN:
			if (state == E_DOWN)
			{
				for (std::vector<UI_Image*>::iterator it = start_menu_selectables.begin(); it != start_menu_selectables.end(); it++)
				{
					if ((*it)->active && (it + 1) != start_menu_selectables.end())
					{
						(*it)->active = false;
						(*(it + 1))->Set_Active_state(true);
						break;
					}
				}
			}
			break;

		case CONFIRM:

			if (start_continue->active)
			{
				//App->LoadGame("saves.xml");
				//App->inputM->SetGameContext(GAMECONTEXT::IN_MENU);
			}

			if (start_new_game->active)
			{
				StartGame();
				App->cutsceneM->StartCutscene(1);
			}

			if (start_quit->active)
			{
				App->game->quit_game = true;
				App->game->pause = false;
			}

			break;	
		}
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
		save = (UI_Image*)LoadUIElement(pause_node.child("save"), main_menu, IMAGE);
		load = (UI_Image*)LoadUIElement(pause_node.child("load"), main_menu, IMAGE);
		controls = (UI_Image*)LoadUIElement(pause_node.child("controls"), main_menu, IMAGE);
		quit = (UI_Image*)LoadUIElement(pause_node.child("quit"), main_menu, IMAGE);

		saved_game = (UI_Image*)LoadUIElement(pause_node.child("saved_game"), main_menu, IMAGE);
		loaded_game = (UI_Image*)LoadUIElement(pause_node.child("loaded_game"), main_menu, IMAGE);

		//Main menu
		start_menu_screen = (UI_Image*)LoadUIElement(pause_node.child("start_menu"), nullptr, IMAGE);
		start_continue = (UI_Image*)LoadUIElement(pause_node.child("continue"), start_menu_screen, IMAGE);
		start_new_game = (UI_Image*)LoadUIElement(pause_node.child("new"), start_menu_screen, IMAGE);
		start_quit = (UI_Image*)LoadUIElement(pause_node.child("start_quit"), start_menu_screen, IMAGE);

		start_menu_selectables.push_back(start_continue);
		start_menu_selectables.push_back(start_new_game);
		start_menu_selectables.push_back(start_quit);

		//selecable items go into vector
		pause_selectables.push_back(resume);
		pause_selectables.push_back(save);
		pause_selectables.push_back(load);
		pause_selectables.push_back(controls);
		pause_selectables.push_back(quit);

		App->gui->CreateScreen(main_menu);
		App->gui->CreateScreen(start_menu_screen);
	
		
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

			tmp->SetImageRect(LoadRect(node));
			tmp->Set_Interactive_Box({ node.attribute("pos_x").as_int(), node.attribute("pos_y").as_int(), 0, 0 });
			tmp->Set_Active_state(node.attribute("active").as_bool());

			if (screen)
				screen->AddChild(ret);

			return ret;
		}

		if (type == STRING)
		{
			UI_String* tmp = (UI_String*)ret;
			tmp->Set_Interactive_Box({ node.attribute("pos_x").as_int(),node.attribute("pos_y").as_int(), 390,0 });
			tmp->Set_String((char*)node.attribute("text").as_string());
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

			tmp->Set_Active_state(node.attribute("active").as_bool());
			tmp->Set_Interactive_Box({ node.attribute("pos_x").as_int(), node.attribute("pos_y").as_int(), 0, 0 });
			tmp->SetDrawRect({ node.attribute("draw_x").as_int() ,  node.attribute("draw_y").as_int() ,0,0 });
			tmp->SetBackground(stamina_container);
			tmp->SetStamina(stamina_green);

			if (screen)
				screen->AddChild(ret);

			return ret;

		}

		if (type == ZELDA_LIFE)
		{
			UI_Stamina* tmp = (UI_Stamina*)ret;

			tmp->Set_Active_state(node.attribute("active").as_bool());
			tmp->Set_Interactive_Box({ node.attribute("pos_x").as_int(), node.attribute("pos_y").as_int(), 0, 0 });
			tmp->SetDrawRect({ node.attribute("draw_x").as_int() ,  node.attribute("draw_y").as_int() ,0,0 });
			tmp->SetBackground(life_container);
			tmp->SetStamina(full_life);

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
	save->Set_Active_state(false);
	load->Set_Active_state(false);
	controls->Set_Active_state(false);
	quit->Set_Active_state(false);

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

void Hud::StartGame()
{
	start_menu_screen->Set_Active_state(false);
	start_menu_screen->QuitFromRender();
	hud_screen->Set_Active_state(true);
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
		item_frame = (UI_Image*)LoadUIElement(hud_node.child("items").child("frame"), hud_screen, IMAGE);
		items_frame_inactive = (UI_Image*)LoadUIElement(hud_node.child("items").child("frame_y"), nullptr, IMAGE);
		items_frame_using = (UI_Image*)LoadUIElement(hud_node.child("items").child("frame_r"), nullptr, IMAGE);

		items_bow = (UI_Image*)LoadUIElement(hud_node.child("items").child("bow"), item_frame, IMAGE);
		items_bomb = (UI_Image*)LoadUIElement(hud_node.child("items").child("bomb"), item_frame, IMAGE);
		items_potion_red = (UI_Image*)LoadUIElement(hud_node.child("items").child("potion_red"), item_frame, IMAGE);
		items_potion_green = (UI_Image*)LoadUIElement(hud_node.child("items").child("potion_green"), item_frame, IMAGE);

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

		//Zeldu life
		life_container = (UI_Image*)LoadUIElement(hud_node.child("zelda").child("container"), nullptr, IMAGE);
		full_life = (UI_Image*)LoadUIElement(hud_node.child("zelda").child("full_health"), nullptr, IMAGE);
		mid_life = (UI_Image*)LoadUIElement(hud_node.child("zelda").child("mid_health"), nullptr, IMAGE);
		low_life = (UI_Image*)LoadUIElement(hud_node.child("zelda").child("low_health"), nullptr, IMAGE);
		zelda_life_bar = (UI_Stamina*)LoadUIElement(hud_node.child("zelda").child("bar"), hud_screen, ZELDA_LIFE);

		LoadHearts();
		SetFrame();

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

void Hud::LookInventory()
{
	if (App->game->em->player->inventory.size() <= 0)
	{
		items_bomb->Set_Active_state(false);
		items_bow->Set_Active_state(false);
		items_potion_red->Set_Active_state(false);
		items_potion_green->Set_Active_state(false);
		return;
	}

	int i = 0;
	std::list<OBJECT_TYPE>::iterator it = App->game->em->player->inventory.begin();
	for (; i < App->game->em->player->equippedObject; i++) { it++; }

	ActiveItem((*it));

	
}

void Hud::ActiveItem(OBJECT_TYPE item)
{
	switch (item)
	{
	case NO_OBJECT:
		items_bomb->Set_Active_state(false);
		items_bow->Set_Active_state(false);
		items_potion_red->Set_Active_state(false);
		items_potion_green->Set_Active_state(false);
		break;

	case BOMB_SAC:
		items_bomb->Set_Active_state(true);
		items_bow->Set_Active_state(false);
		items_potion_red->Set_Active_state(false);
		items_potion_green->Set_Active_state(false);
		break;

	case LIFE_POTION:

		items_bomb->Set_Active_state(false);
		items_bow->Set_Active_state(false);
		items_potion_red->Set_Active_state(true);
		items_potion_green->Set_Active_state(false);
		break;

	case BOW:
		items_bomb->Set_Active_state(false);
		items_bow->Set_Active_state(true);
		items_potion_red->Set_Active_state(false);
		items_potion_green->Set_Active_state(false);
		break;

	case STAMINA_POTION:
		items_bomb->Set_Active_state(false);
		items_bow->Set_Active_state(false);
		items_potion_red->Set_Active_state(false);
		items_potion_green->Set_Active_state(true);
		break;
	}

}

void Hud::SetFrame()
{
	item_frame->Image = items_frame_inactive->Image;
	item_active = false;
}

