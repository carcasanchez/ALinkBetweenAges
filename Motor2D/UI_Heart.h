#ifndef _UI_HEART_H_
#define  _UI_HEART_H_

#include "UI_element.h"

class UI_Image;

enum HEARTSTATE
{
	FULL, 
	MID, 
	EMPTY
};

class UI_Heart : public UI_element
{
public:
	UI_Heart(UI_TYPE, j1Module*);

	bool		Update_Draw();
	bool		Update();

	UI_Image*	heart_img = nullptr;
	HEARTSTATE	h_state;
	int			hit = 4;
};



#endif // ! _UI_HEART_H_

