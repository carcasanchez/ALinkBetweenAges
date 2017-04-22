#include "UI_Heart.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1GameLayer.h"
#include "HUD.h"
#include "UI_Image.h"
#include "j1Render.h"

UI_Heart::UI_Heart(UI_TYPE type, j1Module* callback) : UI_element(type, callback), h_state(FULL_HEART){}

bool UI_Heart::Update_Draw()
{
	if (active)
	{
		if (heart_img != nullptr)
		{
			if (!Parent)
				App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), (Interactive_box.x - App->render->camera.x) * App->gui->scale_factor, (Interactive_box.y - App->render->camera.y) * App->gui->scale_factor, &heart_img->Image);
			else App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), ((Parent->Interactive_box.x + Interactive_box.x) - App->render->camera.x) * App->gui->scale_factor, ((Parent->Interactive_box.y + Interactive_box.y) - App->render->camera.y) * App->gui->scale_factor, &heart_img->Image);
		}
	}
	return true;
}

bool UI_Heart::Update()
{
	
	

	return true;
}

void UI_Heart::ChangeState(HEARTSTATE new_state)
{
	if (new_state != h_state)
	{
		switch (new_state)
		{
		case FULL_HEART:
			h_state = new_state;
			heart_img = App->game->hud->full_heart;
			break;

		case EMPTY_HEART:
			h_state = new_state;
			heart_img = App->game->hud->empty_heart;
			break;
		}
	}
}
