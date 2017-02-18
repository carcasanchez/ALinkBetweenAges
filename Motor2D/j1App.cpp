#include <iostream> 
#include <sstream> 


#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1EntityManager.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1FileSystem.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "j1Console.h"
#include "j1App.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{

	name = "app";
	PERF_START(ptimer);

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	fs = new j1FileSystem();
	map = new j1Map();
	pathfinding = new j1PathFinding();
	entity_manager = new j1EntityManager();
	font = new j1Fonts();
	gui = new j1Gui();
	console = new j1Console();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	
	AddModule(fs);
	AddModule(input);
	AddModule(font);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(gui);

	// scene last
	AddModule(scene);
	AddModule(entity_manager);
	AddModule(console);

	// render last to swap buffer
	AddModule(render);

	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules	
	for (std::list<j1Module*>::reverse_iterator item = modules.rbegin(); item != modules.crend(); item++)
		RELEASE((*item));

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;
		
	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		// self-config
		ret = true;

		//(app_config.child("title").child_value());
		app_config = config.child("app");
		title = app_config.child("title").child_value();
		organization = app_config.child("organization").child_value();

		int cap = app_config.attribute("framerate_cap").as_int(-1);

		if(cap > 0)
		{
			capped_ms = 1000 / cap;
		}
	}

	if(ret == true)
	{
			
		for (std::list<j1Module*>::const_iterator item = modules.begin(); item != modules.cend(); item++)
			ret = (*item)->Awake(config.child((*item)->name.c_str()));
			
	}
	
	/*
	//load config cvars
	pugi::xml_node start = config.child("app");
	while (start != NULL)
	{
		pugi::xml_node cvar = start.child("cvars").first_child();

		while (cvar != NULL)
		{

			p2SString name = cvar.attribute("name").as_string();
			int i = 0;
			for (i; i < strlen(start.name()); i++)
				name.Insert_Char_beggining(i, (start.name() + i));

			name.Insert_Char_beggining(i, ".");

			p2SString description = cvar.attribute("description").as_string();
			p2SString value = cvar.attribute("value").as_string();

			p2SString cv_module = start.name();
			j1Module* callback = Find_module(cv_module.GetString());

			int min_v = cvar.attribute("min").as_int();
			int max_v = cvar.attribute("max").as_int();

			ARGUMENTS_TYPE type;
			p2SString type_v = cvar.attribute("type").as_string();

			if (type_v == "int")
				type = INT_VAR;

			if (type_v == "char")
				type = CHAR_VAR;

			if (type_v == "bool")
				type = BOOL_VAR;

			bool read_only = cvar.attribute("read_only").as_bool();

			CVar* new_cvar = console->Add_CVar(name.GetString(), description.GetString(), value.GetString(), min_v, max_v, callback, type, read_only);

			LOG("Loaded CVAR: %s, in Module: %s", new_cvar->Get_name(), callback->name.GetString());

			cvar = cvar.next_sibling();
		}

		start = start.next_sibling();

	}
	*/


	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(ptimer);
	bool ret = true;


	for (std::list<j1Module*>::const_iterator item = modules.begin(); item != modules.cend(); item++)
		ret = (*item)->Start();


	startup_time.Start();

	PERF_PEEK(ptimer);

	quit = console->Add_Command("quit", this, 0, 0, ARGUMENTS_TYPE::NONE);
	fps = console->Add_Command("fps", this, 0, 0, ARGUMENTS_TYPE::NONE);


	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	char* buf;
	int size = App->fs->Load("config.xml", &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE(buf);

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.ReadSec();
	frame_time.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if(want_to_save == true)
		SavegameNow();

	if(want_to_load == true)
		LoadGameNow();

	// Framerate calculations --

	if(last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[256];
	sprintf_s(title, 256, "Av.FPS: %.2f Last Frame Ms: %u Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %lu ",
			  avg_fps, last_frame_ms, frames_on_last_update, dt, seconds_since_startup, frame_count);
	App->win->SetTitle(title);

	if(capped_ms > 0 && last_frame_ms < capped_ms)
	{
		j1PerfTimer t;
		SDL_Delay(capped_ms - last_frame_ms);
		//LOG("We waited for %d milliseconds and got back in %f", capped_ms - last_frame_ms, t.ReadMs());
	}

	if (logic_dt < LOGICAL_DT)
		logic_dt += dt;

}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;

	j1Module* pModule = nullptr;
	
	for (std::list<j1Module*>::const_iterator item = modules.begin(); item != modules.cend(); item++)
	{
		pModule = (*item);
		if (pModule->active == false) { continue; }
		ret = (*item)->PreUpdate();
	}
		
	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	j1Module* pModule = NULL;

	if (logic_dt >= LOGICAL_DT)
	{
		update_logic = true;
		logic_dt = 0.0f;
	}

	for (std::list<j1Module*>::const_iterator item = modules.begin(); item != modules.cend(); item++)
	{
		pModule = (*item);
		if (pModule->active == false) { continue; }
		if (update_logic) { ret = (*item)->UpdateTicks(); }
		ret = (*item)->Update(dt);
	}


	if (update_logic)
		update_logic = false;

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	j1Module* pModule = NULL;

	for (std::list<j1Module*>::const_iterator item = modules.begin(); item != modules.cend(); item++)
	{
		pModule = (*item);
		if (pModule->active == false) { continue; }
		ret = (*item)->PostUpdate();
	}

	ret = update_stop;
	return ret;
}

j1Module* j1App::Find_module(const char* mod_name)
{
	
	if (strcmp(mod_name, "app") == 0)
		return this;

	std::list<j1Module*>::iterator ret = modules.begin();
	while (ret != modules.end())
	{
		if ((*ret)->name == mod_name)
			return (*ret);

		++ret;
	}

	return nullptr;
}

bool j1App::On_Console_Callback(command* com, int* argument)
{

	if (com == quit)
	{
		//LOG("Quit");
		update_stop = false;
	}

	if (com == fps)
		//LOG("Max Fps: %.2f", 1 / dt);

	return true;
}

bool j1App::On_Console_Callback(CVar* cvar)
{

	if (strcmp(cvar->Get_name(), "app.max_fps") == 0)
	{
		if (cvar->Cvar_type == INT_VAR)
			capped_ms = cvar->Get_value_Int();
	}

	return true;
}

// Called before quitting
bool j1App::CleanUp()
{
	PERF_START(ptimer);
	bool ret = true;
	//p2List_item<j1Module*>* item;
	//item = modules.end;

	std::list<j1Module*>::reverse_iterator item = modules.rbegin();
	
	for (std::list<j1Module*>::reverse_iterator item = modules.rbegin(); item != modules.crend() && ret == true; item++)
		(*item)->CleanUp();


	//PERF_PEEK(ptimer);
	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.c_str();
}

// ---------------------------------------
float j1App::GetDT() const
{
	return dt;
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.c_str();
}

// Load / Save
void j1App::LoadGame(const char* file)
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
	load_game = ("%s%s", fs->GetSaveDirectory(), file);
}

// ---------------------------------------
void j1App::SaveGame(const char* file) const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
	save_game = file;
}

// ---------------------------------------
void j1App::GetSaveGames(list<string>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	char* buffer;
	uint size = fs->Load(load_game.c_str(), &buffer);

	if(size > 0)
	{
		pugi::xml_document data;
		pugi::xml_node root;

		pugi::xml_parse_result result = data.load_buffer(buffer, size);
		RELEASE(buffer);

		if(result != NULL)
		{
			//LOG("Loading new Game State from %s...", load_game.GetString());

			root = data.child("game_state");

			std::list<j1Module*>::iterator item = modules.begin();
			ret = true;

			while(item != modules.end() && ret == true)
			{
				ret = item._Ptr->_Myval->Load(root.child(item._Ptr->_Myval->name.c_str()));
				++item;
			}

			data.reset();
			if(ret == true)
				LOG("...finished loading");
			//else
				//LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown");
		}
		//else
			//LOG("Could not parse game state xml file %s. pugi error: %s", load_game.GetString(), result.description());
	}
	else
		//LOG("Could not load game state xml file %s", load_game.GetString());

	want_to_load = false;
	return ret;
}

bool j1App::SavegameNow() const
{
	bool ret = true;

	//LOG("Saving Game State to %s...", save_game.GetString());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;
	
	root = data.append_child("game_state");

	//p2List_item<j1Module*>* item = modules.start;
	std::list<j1Module*>::const_iterator item;
	item = modules.begin();


	while(item != modules.end() && ret == true)
	{
		ret = item._Ptr->_Myval->Save(root.append_child(item._Ptr->_Myval->name.c_str()));
		++item;
	}

	if(ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
		fs->Save(save_game.c_str(), stream.str().c_str(), stream.str().length());
		//LOG("... finished saving", save_game.GetString());
	}
	else
		//LOG("Save process halted from an error in module %s", (item != NULL) ? item->data->name.GetString() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}