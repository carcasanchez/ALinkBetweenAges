#ifndef _UI_IMAGE_H_
#define _UI_IMAGE_H_

#include "UI_element.h"
#include "SDL\include\SDL_rect.h"

class UI_Image : public UI_element
{
public:
	SDL_Rect Image;
	int id = -1;
	

public:

	UI_Image(UI_TYPE, j1Module*);
	UI_Image(UI_TYPE type, SDL_Rect detection_box, SDL_Rect img_size, bool active = true, SCROLL_TYPE draggable = FREE_SCROLL, int id = -1);
	UI_Image(const UI_Image* other);

	bool Update();
	bool Update_Draw();
	bool Handle_input();
	void change_image(SDL_Rect new_image);

	void Set_Image_Texture(SDL_Rect);

};


#endif // !_UI_IMAGE_H_

