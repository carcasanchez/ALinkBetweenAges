#ifndef _UI_DIALOG_BOX_H_
#define _UI_DIALOG_BOX_H_

#include "j1Module.h"
#include "UI_element.h"
#include "UI_Image.h"
#include "UI_String.h"

class UI_DialogBox : public UI_element
{
public:

	UI_DialogBox(UI_TYPE, j1Module*);
	~UI_DialogBox();

	void SetImage(const UI_Image*);
	void SetText(const UI_String*);
	void SetViewport(SDL_Rect);

private:

	UI_Image*	Box = nullptr;
	UI_String*	Dialog_Text = nullptr;
	SDL_Rect	Dialog_ViewPort;
};



#endif // !_UI_DIALOG_BOX_H_
