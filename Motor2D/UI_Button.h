#ifndef _UI_BUTTON_H_
#define _UI_BUTTON_H_

#include "UI_element.h"
#include "UI_Image.h"

class UI_Button : public UI_element
{
public:
	UI_Image* Button_image;

public:

	UI_Button(UI_TYPE , j1Module*);
	UI_Button(UI_TYPE type, SDL_Rect detection_box, UI_Image* start_image, bool active = true, SCROLL_TYPE draggable = FREE_SCROLL);
	UI_Button(const UI_Button* other);

	void Set_Button_Image(const UI_Image* new_image);	//Changes the Button image
	bool Update();
	bool Update_Draw();
	bool Handle_input();
};


#endif // !_UI_BUTTON_H_
