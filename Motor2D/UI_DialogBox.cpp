#include "UI_DialogBox.h"

UI_DialogBox::UI_DialogBox(UI_TYPE typ, j1Module* callback) : UI_element(typ, callback) {}

UI_DialogBox::~UI_DialogBox()
{
	RELEASE(Box);
}

void UI_DialogBox::SetImage(const UI_Image* new_img)
{
	Box = (UI_Image*)new_img;
}

void UI_DialogBox::SetText(const UI_String* new_txt)
{
	Dialog_Text = (UI_String*)new_txt;
}

void UI_DialogBox::SetViewport(SDL_Rect new_rect)
{
	Dialog_ViewPort = new_rect;
}
