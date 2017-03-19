#define _CRT_SECURE_NO_WARNINGS
#include "j1Console.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "UI_Image.h"
#include "p2Log.h"
#include "j1Render.h"
#include "CVar_Com.h"

//--------------CONSOLE-------------------
//----------------------------------------

j1Console::j1Console()
{
	name = ("console");
}

bool j1Console::Awake(pugi::xml_node& config)
{
	LOG("Setting Console features");
	
	console_screen.x = config.child("Position").attribute("x").as_int();
	console_screen.y = config.child("Position").attribute("y").as_int();
	console_screen.w = config.child("Size").attribute("width").as_int();
	console_screen.h = config.child("Size").attribute("height").as_int();

	Background.r = config.child("Color").attribute("r").as_int();
	Background.g = config.child("Color").attribute("g").as_int();
	Background.b = config.child("Color").attribute("b").as_int();
	Background.a = config.child("Color").attribute("a").as_int();

	
	LOG("Desactivating console");
	active = false;

	return true;
}

bool j1Console::Start()
{

	LOG("Setting console text box");

	int width;
	App->font->CalcSize("Set", width, height, App->font->default);

	
	Input_text = (UI_Text_Box*)App->gui->Add_element(UI_TYPE::TEXT_BOX, this);
	Input_text->Set_Interactive_Box({ console_screen.x, (int)((console_screen.y + console_screen.h)), console_screen.w, height });
	Input_text->Set_Drag_Type(NO_SCROLL);

	help = Add_Command("help", this, 0, 0, NONE);
	CV_list = Add_Command("cv_list", this, 0, 0, NONE);

	
	return true;
}

bool j1Console::Update(float dt)
{

	Load_Update_labels();
	Input_text->Update();

	check_state();

	if (state == MOUSE_OVER)
		drag_console();


	return true;
}

bool j1Console::PostUpdate()
{
	App->render->DrawQuad({ (int)((console_screen.x - App->render->camera.x)* App->gui->scale_factor) , (int)((console_screen.y - App->render->camera.y)* App->gui->scale_factor) , console_screen.w, (int)(console_screen.h* App->gui->scale_factor) }, Background.r, Background.g, Background.b, Background.a);
	App->render->DrawQuad({ (int)((Input_text->Interactive_box.x - App->render->camera.x)* App->gui->scale_factor),  (int)((Input_text->Interactive_box.y - App->render->camera.y) * App->gui->scale_factor) + 100, Input_text->Interactive_box.w, Input_text->Interactive_box.h }, 0, 0, 255, Background.a);

	//Change viewport
	SDL_RenderSetViewport(App->render->renderer, &console_screen);

	for (int i = 0; i < num_of_labels; i++)
		App->render->Blit(Labels[i]->text_texture, (Labels[i]->Interactive_box.x - App->render->camera.x)* App->gui->scale_factor, (Labels[i]->Interactive_box.y));
				
	SDL_RenderSetViewport(App->render->renderer, nullptr);



	Input_text->Update_Draw();
	return true;
}

bool j1Console::CleanUp()
{
	UI_String* result;

	for (; labels_loaded >= 0; labels_loaded--)
		Labels.pop_back();

	delete Input_text;
	return true;
}

void j1Console::Active_console()
{
	if (App->console->active)
	{
		App->console->active = false;
		SDL_StopTextInput();
	}
	else
	{
		App->console->active = true;
		App->gui->element_selected = Input_text;
		
	}
}

void j1Console::Add_Label(const char * new_text)
{
	UI_String* new_label = new UI_String(UI_TYPE::STRING, { 0,0,0,0 }, (char*)new_text);
	
	if (new_label)
		Labels.push_back(new_label);

	num_of_labels++;
}

command* j1Console::Add_Command(const char * comm, j1Module * callback, uint min_arg, uint max_args, ARGUMENTS_TYPE type)
{
	command* new_command = new command(comm, callback, min_arg, max_args, type);

	if (new_command)
		Commands_List.push_back(new_command);

	return new_command;
}

CVar * j1Console::Add_CVar(const char* nam, const char* des, const char* default_v, int min_v, int max_v, j1Module* cb, ARGUMENTS_TYPE cvtype, bool ro)
{
	CVar* ret = new CVar(nam, des, default_v, min_v, max_v, cb, cvtype, ro);

	if (ret)
		CVars_list.push_back(ret);

	return ret;
}

bool j1Console::On_GUI_Callback(UI_element* ui_element, GUI_INPUT type)
{
	switch (type)
	{
	case MOUSE_IN:
		break;
	case MOUSE_OUT:
		break;
	case RIGHT_MOUSE_DOWN:
		break;
	case RIGHT_MOUSE_REPEAT:
		break;
	case LEFT_MOUSE_DOWN:
		break;
	case LEFT_MOUSE_REPEAT:
		break;
	case ENTER:
		if (ui_element == Input_text)
		{
			Camera_management();
			Text_management();
		}
		break;
	}

	return true;
}

bool j1Console::On_Console_Callback(command* callback_com)
{
	if (callback_com == help)
	{
		LOG("-List of commands:");
		int num_comm = Commands_List.size() - 1;

		for (; num_comm >= 0; num_comm--)
			LOG("-/%s   %i values", Commands_List[num_comm]->name.c_str(), Commands_List[num_comm]->max_arguments);

	}

	if (callback_com == CV_list)
	{
		LOG("-List of CVars:");
		int num_cv = CVars_list.size() - 1;

		for (; num_cv >= 0; num_cv--)
			LOG("-%s ", CVars_list[num_cv]->Get_name());

	}


	return true;
}

void j1Console::check_state()
{
	int x, y;
	App->input->GetMousePosition(x, y);

	if (((x > console_screen.x) && (x <= (console_screen.x + console_screen.w)) && (y > console_screen.y) && (y <= (console_screen.y + console_screen.h))))
		state = MOUSE_OVER;
	else state = MOUSE_NOT_OVER;
	
}

void j1Console::drag_console()
{
	int y;
	App->input->GetMouseWheel(y);

	for(int i = 0; i < num_of_labels; i++)
		Labels[i]->Interactive_box.y += y;

}

void j1Console::Load_labels()
{
	
	for (; labels_loaded < num_of_labels; labels_loaded++)
	{
		Labels[labels_loaded]->Load_text_texture();

		if(labels_loaded > 0)
			Labels[labels_loaded]->Interactive_box.y = Labels[labels_loaded - 1]->Interactive_box.y + height;
	}
	
	
}

void j1Console::Load_Update_labels()
{
	if (num_of_labels != labels_loaded)
		Load_labels();
}

void j1Console::Camera_management()
{
	int last_label = (Labels[num_of_labels - 1]->Interactive_box.y);

	if (last_label > console_screen.h)
	{
		for (int i = 0; i < num_of_labels; i++)
			Labels[i]->Interactive_box.y += console_screen.y - last_label;
	}
}

void j1Console::Text_management()
{
	//If there are spaces before the command/cvar they are ignored
	int first_letter = Input_text->text.text.find_first_not_of(" ");

	//if is a command:
	if (Input_text->text.text[first_letter] == '/')
	{
		command* tmp = Command_management(Input_text->text.text.substr(first_letter + 1).c_str());

		if(tmp != nullptr)
			tmp->my_module->On_Console_Callback(tmp);
	}
	else
	{
		CVar* tmp = Cvar_management(Input_text->text.text.substr(first_letter).c_str());

		if (tmp != nullptr)
			tmp->Callback->On_Console_Callback(tmp);
			
	}

}

command* j1Console::Command_management(const char* Input_command)
{
	
	int num_of_commands = Commands_List.size();
	for (int i = 0; i < num_of_commands; i++)
	{
		if (Commands_List[i]->name == Input_command)
			return Commands_List[i];		
	}
	
	LOG("ERROR: command does not exist");
	return nullptr;
}

CVar* j1Console::Cvar_management(const char* input_text)
{
	//Find the CVar
	string input_CVar = input_text;
	int value_pos = input_CVar.find_first_of(" ", 0);
	string cvar_name = input_CVar.substr(0, value_pos);

	vector<CVar*>::iterator item = CVars_list.begin();
	for (; item != CVars_list.end(); item++)
	{
		if ((*item)->Get_name() == cvar_name)
			break;
	}

	//if does not exists return nullptr
	if (item == CVars_list.end())
	{
		LOG("ERROR: CVar does not exist");
		return nullptr;
	}
	//if it does change the cvar value for the new one
	else
	{
		string values;
		if(value_pos < input_CVar.size())
			values = input_CVar.substr(value_pos);
	
		bool changed_value = CvarValueAssigment((*item), values);
		return changed_value ? (*item) : nullptr;
	}
}

bool j1Console::CvarValueAssigment(CVar* cv, string values)
{
	if (values.empty())
	{
		string RO;	//Means Read only
		cv->Get_RO() ? RO = "true" : RO = "false";
		LOG("Description: %s, Actual value: %s, Min value: %i, Max value: %i, Read only: %s", cv->Get_Description(), cv->Get_value_Char(), cv->Get_min(), cv->Get_max(), RO.c_str());
		return false;
	}

	//FOR NOW CVARS ACCEPT 1 VALUE
	int new_value = atoi(values.c_str());
	if (new_value <= cv->Get_max() && new_value >= cv->Get_min())
	{
		cv->Set_value(values.c_str());
		return true;
	}
	else
	{
		LOG("Value is not valid! Values accepted are between [%i-%i]", cv->Get_min(), cv->Get_max());
		return false;
	}
		

}

//---------------COMAND-------------------
//----------------------------------------

command::command(const char* new_com, j1Module* callback, unsigned int min_args, unsigned int max_args, ARGUMENTS_TYPE type) : name(new_com), my_module(callback), min_arguments(min_args), max_arguments(max_args), args_type(type) {}

//----------------CVAR--------------------
//----------------------------------------

CVar::CVar(const char* nam, const char* des, const char* default_v, int min_v, int max_v, j1Module* cb, ARGUMENTS_TYPE cvtype, bool ro) : name(nam), description(des), value(default_v), min_value(min_v), max_value(max_v), Callback(cb), Cvar_type(cvtype), read_only(ro) {}