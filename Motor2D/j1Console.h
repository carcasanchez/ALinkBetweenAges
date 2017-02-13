#ifndef _J1CONSOLE_H_
#define _J1CONSOLE_H_

#include "j1Module.h"
#include "CVar_Com.h"
#include "UI_Text_Box.h"
#include "p2SString.h"
#include "p2DynArray.h"
#include "SDL\include\SDL_rect.h"

struct _TTF_Font;
struct UI_String;
struct UI_Image;


//--------------CONSOLE-------------------
//----------------------------------------

enum CONSOLE_STATE
{
	MOUSE_OVER,
	MOUSE_NOT_OVER
};

class j1Console : public j1Module
{
public:

	j1Console();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void Active_console();
	void Add_Label(const char* );

	command* Add_Command(const char* , j1Module* , uint , uint, ARGUMENTS_TYPE);
	CVar* Add_CVar(const char*, const char* , const char*, int , int , j1Module* , ARGUMENTS_TYPE , bool );

	bool On_GUI_Callback(UI_element*, GUI_INPUT);

	bool On_Console_Callback(command*, int*);
	bool On_Console_Callback(command*, char*);

public:

	SDL_Rect console_screen;
	SDL_Color Background;
	CONSOLE_STATE state;

	UI_Text_Box* Input_text;

	p2DynArray<UI_String*> Labels;
	p2DynArray<command*> Commands_List;
	p2DynArray<CVar*> CVars_list;

private:

	int height;
	int num_of_labels = 0;
	int labels_loaded = 0;

	void check_state();
	void drag_console();
	void Load_labels();
	void Load_Update_labels();

	void Camera_management();

	void Text_management();

	void Argument_management(const char*, int, command*);
	void Value_CV_management(const char*, int, CVar*);

	command* Command_management(const char*);
	CVar* Cvar_management(const char*);

	command* help;
	command* CV_list;
	

};



#endif
