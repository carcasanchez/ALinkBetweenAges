#include "j1App.h"
#include "j1Gui.h"
#include "j1Render.h"
#include "UI_Button.h"


UI_Button::UI_Button(UI_TYPE type, j1Module* module) : UI_element(type, module) {}

UI_Button::UI_Button(UI_TYPE type, SDL_Rect detection_box,  UI_Image* start_image, bool act, SCROLL_TYPE drag) : UI_element(type, detection_box, act, drag), Button_image(start_image)
{
	
}

UI_Button::UI_Button(const UI_Button* other) : UI_element(other->element_type, other->Interactive_box, other->active, other->draggable), Button_image(other->Button_image)
{
	
}

void UI_Button::Set_Button_Image(const UI_Image* new_image)
{	
	Button_image = (UI_Image*)new_image;	
}

bool UI_Button::Update_Draw()
{

	if (active)
	{
		App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), (Interactive_box.x - App->render->camera.x) * App->gui->scale_factor, (Interactive_box.y - App->render->camera.y)* App->gui->scale_factor, &Button_image->Image);

		Child_Update_Draw();
	}
	return true;
}

bool UI_Button::Handle_input()
{
	int x, y;
	App->input->GetMousePosition(x, y);

	if (this->Mouse_is_in({ x, y }))
	{
		my_module->On_GUI_Callback(this, MOUSE_IN);
		state = OVER_ELEMENT;
	}
	else
	{
		my_module->On_GUI_Callback(this, MOUSE_OUT);
		state = NOTHING;
	}
	
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && App->gui->element_selected == nullptr)
	{
		if(this->Mouse_is_in({ x, y }))
		{
			my_module->On_GUI_Callback(this, LEFT_MOUSE_DOWN);
			App->gui->element_selected = this;
			App->gui->focus_element = this;
			state = CLICK_ELEMENT;
		}
		else
		{
			App->gui->element_selected = nullptr;
			App->gui->focus_element = nullptr;
			state = NOTHING;
		}
	}
	
	if (App->gui->element_selected != this) return true;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		my_module->On_GUI_Callback(this, LEFT_MOUSE_REPEAT);
		state = CLICK_ELEMENT;
	}
	

	return true;
}

bool UI_Button::Update()
{
	Handle_input();

	if (App->gui->element_selected == this && draggable)
		Drag_element();

	Child_Update();

	Return_state();

	return true;
}