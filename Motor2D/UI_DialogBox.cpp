#include "UI_DialogBox.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Gui.h"

UI_DialogBox::UI_DialogBox(UI_TYPE typ, j1Module* callback) : UI_element(typ, callback) {}

UI_DialogBox::~UI_DialogBox()
{
	RELEASE(Box);
}

bool UI_DialogBox::Update_Draw()
{
	//blit img
	if (Box)
	{
		if (!Parent)
			App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), (Interactive_box.x - App->render->camera.x) * App->gui->scale_factor, (Interactive_box.y - App->render->camera.y) * App->gui->scale_factor, &Box->Image);
		else App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), ((Parent->Interactive_box.x + Interactive_box.x) - App->render->camera.x) * App->gui->scale_factor, ((Parent->Interactive_box.y + Interactive_box.y) - App->render->camera.y) * App->gui->scale_factor, &Box->Image);
	}
	Child_Update_Draw();
	return false;
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
