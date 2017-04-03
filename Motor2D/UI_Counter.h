#ifndef _UI_COUNTER_H_
#define _UI_COUNTER_H_

#include "UI_element.h"
#include "UI_Image.h"

class UI_Counter : public UI_element
{
public:
	UI_Counter(UI_TYPE, j1Module*);

	int cent = 0;
	int dec = 0;
	int unit = 0;

	UI_Image* img = nullptr;

	int rows = 0;
	int columns = 0;

	int width = 0;
	int height = 0;

};


#endif
