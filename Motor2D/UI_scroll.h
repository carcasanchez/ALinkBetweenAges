#ifndef _UI_SCROLL_H_
#define _UI_SCROLL_H_

#include "UI_element.h"
#include "p2Point.h"
#include "UI_Image.h"


class UI_Scroll : public UI_element
{
private:

	UI_Image* Slider;
	UI_Image* Slide_box;
	SDL_Rect Stop_box;

	SDL_Rect Camera;
	SDL_Rect Camera_inner_box;

	list<UI_element*> Camera_elements;
	list<iPoint> Camera_element_position;



public:

	UI_Scroll(UI_TYPE, j1Module*);
	UI_Scroll(UI_TYPE type, SDL_Rect detection_box, const UI_Image* slider, const UI_Image* slide_box, bool active = true, SCROLL_TYPE draggable = FREE_SCROLL);
	UI_Scroll(const UI_Scroll* other);
	~UI_Scroll();

	 bool Update();
	 bool Update_Draw();
	 bool Handle_input();

	 void Stop();
	 void Move_elements();
	 void Move_stop_box();
	 void Scroll_Wheel();

	 bool Mouse_inside_Camera_box();

	 void Add_Camera_element(UI_element* new_item);
	 SDL_Rect Get_Stop_Box() const;

	 void Set_Slider_image(UI_Image*);			//Set a image to the slider
	 void Set_Stop_box_image(UI_Image*);		//Set a image to the stop box
	 void Set_Stop_Box(SDL_Rect new_box);		//Set the Box that colides with the slider
	 void Set_Camera(SDL_Rect new_box);			//Set the viewport

};


#endif // !_UI_SCROLL_H_


