#ifndef __j1APP_H__
#define __j1APP_H__


#include <list>
#include "p2List.h"
#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "PugiXml\src\pugixml.hpp"

#define LOGICAL_DT 0.1f

// Modules
class j1FileSystem;
class j1Input;
class InputManager;
class j1Fonts;
class j1Window;
class j1Textures;
class j1Audio;
class j1Map;
class j1PathFinding;
class j1ParticleManager;
class j1Gui;
class j1SceneManager;
class j1CollisionManager;
class j1GameLayer;
class j1Console;
class j1Render;
class DialogManager;

class j1App : public j1Module
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(j1Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;
	float GetDT() const;

	void LoadGame(const char* file);
	void SaveGame(const char* file) const;
	void GetSaveGames(list<string>& list_to_fill) const;

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	j1Module* Find_module(const char*);

	//bool On_Console_Callback(command*, int*);
	bool On_Console_Callback(CVar*);
	bool On_Console_Callback(command*);

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

	command* quit;
	command* fps;
	

public:

	// Modules
	j1FileSystem*		fs = NULL;
	j1Input*			input = NULL;
	InputManager*		inputM = NULL;
	j1Fonts*			font = NULL;
	j1Window*			win = NULL;
	j1Textures*			tex = NULL;
	j1Audio*			audio = NULL;
	j1Map*				map = NULL;
	j1PathFinding*		pathfinding = NULL;
	j1ParticleManager*	particles = NULL;
	j1Gui*				gui = NULL;
	j1SceneManager*		sceneM = NULL;
	j1CollisionManager*	collisions = NULL;
	j1GameLayer*		game = NULL;
	j1Console*			console = NULL;
	j1Render*			render = NULL;
	DialogManager*      dialog = NULL;
private:

	list<j1Module*>	modules;
	int					argc;
	char**				args;

	string				title;
	string				organization;

	mutable bool		want_to_save = false;
	bool				want_to_load = false;
	string				load_game;
	mutable string		save_game;

	j1PerfTimer			ptimer;
	j1PerfTimer			delay_timer;

	uint64				frame_count = 0;
	j1Timer				startup_time;
	j1Timer				frame_time;
	j1Timer				last_sec_frame_time;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;
	float				dt = 0.0f;
	int					capped_ms = -1;

	bool				update_stop = true;

	float				logic_dt = 0.0f;
	bool				update_logic = false;
};

extern j1App* App; // No student is asking me about that ... odd :-S

#endif