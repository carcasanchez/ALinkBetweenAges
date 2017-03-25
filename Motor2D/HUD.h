#ifndef _HUD_H_
#define _HUD_H_

#include "j1Module.h"
#include "UI_Image.h"

class UI_element;
class UI_Image;

class Hud
{
public:
	Hud();

	bool Awake(pugi::xml_node& conf);
	bool Start();
	
	bool Update();

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
	UI_element*		pause_screen = nullptr;
	UI_Image*		main_menu = nullptr;
	UI_Image*		item_menu = nullptr;
	UI_Image*		resume = nullptr;
	UI_Image*		quit = nullptr;

	

private:

	bool		LoadPause(string file);
	SDL_Rect	LoadRect(pugi::xml_node);

	void		SetPauseElements();

};

#endif // !_HUD_H_
