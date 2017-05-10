#define _CRT_SECURE_NO_WARNINGS
#include "UI_String.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Fonts.h"

UI_String::UI_String(UI_TYPE type, j1Module* callback) : UI_element(type, callback) {}

UI_String::UI_String(UI_TYPE type, SDL_Rect detection_box, char* new_text, bool act, SCROLL_TYPE drag) : UI_element(type, detection_box, act, drag), text(new_text) {}

UI_String::~UI_String()
{
	if (text_texture)
		App->tex->UnLoad(text_texture);
}

bool UI_String::Update_Draw()
{
	if (active)
	{
		SDL_Rect tmp = {0, 0, 405,75 };
		App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), (Interactive_box.x - App->render->camera.x - 15) * App->gui->scale_factor, (Interactive_box.y - App->render->camera.y - 15) * App->gui->scale_factor, &tmp);
		App->render->Blit(text_texture, (Interactive_box.x - App->render->camera.x) * App->gui->scale_factor, (Interactive_box.y - App->render->camera.y) * App->gui->scale_factor);
		
	}
		
	Child_Update_Draw();

	return true;
}

bool UI_String::Handle_input()
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

	if (App->gui->element_selected == nullptr) return true;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		my_module->On_GUI_Callback(this, LEFT_MOUSE_REPEAT);
		state = CLICK_ELEMENT;
	}

	
}



void UI_String::SetBlitTimeMS(int time)
{
	blit_time = time;
}

void UI_String::ForcedFinish()
{
	blit_text = text;
	if (text_texture)
		App->tex->UnLoad(text_texture);

	text_texture = App->font->Print(blit_text.c_str(), Interactive_box.w);
}

bool UI_String::Update()
{
	Handle_input();

	if (dialog_state == MID_TEXT && active)
		BlitDialog();

	Return_state();

	return true;
}

bool UI_String::Set_String(char* new_text)
{
	text = new_text;


	blit_text.clear();

	if (text_texture)
		App->tex->UnLoad(text_texture);

	char_blit_time.Start();
	dialog_state = MID_TEXT;

	return (text.c_str() != nullptr) ? true : false;
}

void UI_String::Load_text_texture()
{
	text_texture = App->font->Print(text.c_str(), Interactive_box.w);
}


void UI_String::BlitDialog()
{
	if (blit_text.size() < text.size())
	{
		if(char_blit_time.Read() >= (blit_time))
		{ 
			blit_text += text.at(blit_text.size());
			//looks if the text is already loaded and unloads
			if (text_texture)
				App->tex->UnLoad(text_texture);

			text_texture = App->font->Print(blit_text.c_str(), Interactive_box.w);
			char_blit_time.Start();
		}	
	}
	else dialog_state = FINISHED_TEXT;
	

}

