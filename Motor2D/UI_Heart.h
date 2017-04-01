#ifndef _UI_HEART_H_
#define  _UI_HEART_H_

#include "UI_element.h"

class UI_Image;

class UI_Heart : public UI_element
{
public:
	UI_Heart(UI_TYPE, j1Module*);

	UI_Image*	heart_img = nullptr;
	bool		Update();
	bool		empty = false; 
	int			hit = 4;
};



#endif // ! _UI_HEART_H_

