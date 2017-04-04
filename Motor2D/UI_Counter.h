#ifndef _UI_COUNTER_H_
#define _UI_COUNTER_H_

#include "UI_element.h"
#include "UI_Image.h"

class UI_Counter : public UI_element
{
public:
	UI_Counter(UI_TYPE, j1Module*);
	
	bool Update_Draw();

	void SumNumber(int);
	void SetImage(UI_Image*, int, int);
	void Reset();

private:

	int cent = 0;
	int dec = 0;
	int unit = 0;

	int width = 0;
	int height = 0;

	int rows = 0;
	int columns = 0;

	UI_Image* img = nullptr;

	void BlitUnit();
	void BlitDec();
	void BlitCent();

};


#endif
