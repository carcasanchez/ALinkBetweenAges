#ifndef _UI_IMAGE_H_
#define _UI_IMAGE_H_

#include "UI_element.h"
#include "SDL\include\SDL_rect.h"

class SDL_Texture;

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

	void SetImageRect(SDL_Rect);	//Changes the image rect, not his interactive box
	void SetTexture(SDL_Texture*);

private:

	SDL_Texture* image_texture = nullptr;

};


#endif // !_UI_IMAGE_H_

