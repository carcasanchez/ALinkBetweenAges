#include "UI_Image.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Gui.h"

UI_Image::UI_Image(UI_TYPE type, j1Module* callback) : UI_element(type, callback), Image({ 0,0,0,0 }) {}

UI_Image::UI_Image(UI_TYPE type, SDL_Rect detection_box, SDL_Rect img_size, bool act, SCROLL_TYPE drag, int new_id) : UI_element(type, detection_box, act, drag), Image(img_size), id(new_id) {}

UI_Image::UI_Image(const UI_Image* other) : UI_element(other->element_type, other->Interactive_box, other->active, other->draggable), Image(other->Image), id(other->id) {}

bool UI_Image::Update_Draw()
{
	if (active)
	{
		//Looks if is an IMAGE o an IMAGE_NOT_IN_ATLAS and blits the texture needed
		if (element_type == IMAGE)
			App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), (Interactive_box.x - App->render->camera.x) * App->gui->scale_factor, (Interactive_box.y - App->render->camera.y) * App->gui->scale_factor, &Image);

		else App->render->Blit((SDL_Texture*)App->gui->Get_Other_Textures(id), (Interactive_box.x - App->render->camera.x) * App->gui->scale_factor, (Interactive_box.y - App->render->camera.y) * App->gui->scale_factor, &Image);
	}

	Child_Update_Draw();

	return true;
}

bool UI_Image::Handle_input()
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
		if (this->Mouse_is_in({ x, y }))
		{

			if (get_higher_child() != nullptr)
			{
				state = OVER_ELEMENT;
			}
			else
			{
				my_module->On_GUI_Callback(this, LEFT_MOUSE_DOWN);
				App->gui->element_selected = this;
				App->gui->focus_element = this;
				state = CLICK_ELEMENT;
			}
		}
		else
		{
			App->gui->element_selected = nullptr;
			App->gui->focus_element = nullptr;
			state = NOTHING;
		}
	}

	
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		my_module->On_GUI_Callback(this, LEFT_MOUSE_REPEAT);
		state = CLICK_ELEMENT;
	}

	
	return true;
}

bool UI_Image::Update()
{
	Handle_input();

	if (App->gui->element_selected == this && draggable)
		Drag_element();

	Child_Update();

	Return_state();
	return true;
}

void UI_Image::Set_Image_Texture(SDL_Rect tex)
{
	Image = tex;
}
