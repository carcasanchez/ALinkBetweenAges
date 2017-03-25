#ifndef _HUD_H_
#define _HUD_H_

#include "j1Module.h"
#include "UI_Image.h"
#include "InputManager.h"

class UI_element;
class UI_Image;

class Hud : public InputListener 
{
public:
	Hud();

	bool Awake(pugi::xml_node& conf);
	bool Start();
	bool Update(float);

	void OnInputCallback(INPUTEVENT, EVENTSTATE);

public:
	//In game HUD elements
	UI_element*		hud_screen;

	//Little items
	UI_Image*		Rupees;
	UI_Image*		Bombs;
	UI_Image*		Arrows;

	//life images
	UI_Image*		life;	//Letters up hearts
	UI_Image*		empty_heart;
	UI_Image*		medium_heart;
	UI_Image*		full_heart;

	//items images
	UI_Image*		items_frame;
	UI_Image*		items_bow;
	UI_Image*		items_bomb;
	UI_Image*		items_bumerang;
	UI_Image*		items_potion_red;
	UI_Image*		items_potion_green;

	//Stamina images
	UI_Image*		stamina_bar;
	UI_Image*		stamina;
	UI_Image*		stamina_end;

	//Pause Elements
	bool			pause_transition = false;

	UI_element*		pause_screen = nullptr;
	UI_Image*		main_menu = nullptr;
	UI_Image*		item_menu = nullptr;
	UI_Image*		resume = nullptr;
	UI_Image*		quit = nullptr;

	std::vector<UI_Image*> pause_selectables;

private:

	SDL_Rect	LoadRect(pugi::xml_node);

	//Pause methods
	bool		LoadPause(string file);
	void		SetPauseElements();
	void		IntoPause();
	void		PauseIn(float);

};

#endif // !_HUD_H_
