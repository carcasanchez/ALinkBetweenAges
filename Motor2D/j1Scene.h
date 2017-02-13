#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "p2Point.h"

struct SDL_Texture;
class UI_element;
class UI_Image;
class UI_Button;
class UI_Interactive_String;
class UI_String;
class UI_Scroll;
class UI_Text_Box;
class GuiText;
class Player;


class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool On_GUI_Callback(UI_element* elem, GUI_INPUT gui_input);

private:
	SDL_Texture* debug_tex;
	
	UI_Image* lol;

	//Exam 

	UI_element* Exam_screen;
	UI_Image* Background;

	UI_Image* button_image;
	UI_Image* button_over;
	UI_Image* clicked_Button;

	UI_Image* window;
	UI_String* title;
	UI_Button* Button;
	UI_String* Button_Label;

	UI_Scroll* Heroe_selector;
	UI_Image* Slider_image;
	UI_Image* Heroe_images;
	UI_Image* Big_heroe_image;


	Player* soldier0;
	Player* soldier1;
	Player* soldier2;
	Player* soldier3;
		
};

#endif // __j1SCENE_H__