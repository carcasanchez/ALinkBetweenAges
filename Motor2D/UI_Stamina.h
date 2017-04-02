#ifndef _UI_STAMINA_H_
#define _UI_STAMINA_H_

#include "UI_element.h"
#include "UI_Image.h"


class UI_Stamina : public UI_element
{
public:

	UI_Stamina(UI_TYPE, j1Module*);

	bool		UpdateDraw();

	bool		SetBackground(UI_Image*);
	bool		SetStamina(UI_Image*);
	bool		SetDrawRect(SDL_Rect);

private:

	UI_Image*	background = nullptr;
	UI_Image*	stamina = nullptr;

	SDL_Rect	draw_rect;
	SDL_Rect	move_rect;

};


#endif // !_UI_STAMINA_H_
