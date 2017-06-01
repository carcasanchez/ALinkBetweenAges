#include "UI_Stamina.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1GameLayer.h"
#include "j1EntityManager.h"
#include "HUD.h"
#include "DarkZelda.h"
#include "Player.h"
#include "j1App.h"

UI_Stamina::UI_Stamina(UI_TYPE type, j1Module* callback) : UI_element(type, callback) {}

bool UI_Stamina::Update()
{
	
	RecoverStamina();

	return true;
}

bool UI_Stamina::Update_Draw()
{

	if (active)
	{
		if (Parent)
		{
			App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), ((Parent->Interactive_box.x + Interactive_box.x) - App->render->camera.x) * App->gui->scale_factor, ((Parent->Interactive_box.y + Interactive_box.y) - App->render->camera.y) * App->gui->scale_factor, &background->Image);

			PrintBar();
		}
		else
		{
			App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), (Interactive_box.x - App->render->camera.x) * App->gui->scale_factor, (Interactive_box.y - App->render->camera.y) * App->gui->scale_factor, &background->Image);
			App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), ((draw_rect.x - move_rect.x) - App->render->camera.x) * App->gui->scale_factor, (draw_rect.y - App->render->camera.y) * App->gui->scale_factor, &stamina->Image);
		}


	}

	return true;
}

bool UI_Stamina::SetBackground(UI_Image* new_background)
{
	if (new_background)
	{
		background = new_background;
		return true;
	}
	return false;
}

bool UI_Stamina::SetStamina(UI_Image* green_stamina)
{
	if (green_stamina)
	{
		stamina = green_stamina;
		return true;
	}
	return false;
}

bool UI_Stamina::SetDrawRect(SDL_Rect rect)
{
	draw_rect = move_rect = { Interactive_box.x + rect.x, Interactive_box.y + rect.y, rect.w, rect.h };

	return true;
}

void UI_Stamina::WasteStamina(int tax)
{
	//Calculate percent of move
	float percent = tax / App->game->em->player->maxStamina;

	//move the bar
	int full_move = 160;
	int move = ceil(full_move * percent);

	move_rect.x -= move;

}

void UI_Stamina::RecoverStamina()
{
	
	if (this->element_type == STAMINA_BAR)
	{
		int percent_stamina = (App->game->em->player->stamina * 100) / App->game->em->player->maxStamina;
		ChangeColor(percent_stamina);

		//magic numbers :c
		int image_width = 160;
		int move_to = image_width - ((percent_stamina * image_width) / 100);
		move_rect.x = draw_rect.x - move_to;
		return;
	}

	if (this->element_type == ZELDA_LIFE && active)
	{
		DarkZelda* tmp =(DarkZelda*)App->game->em->GetEntityFromId(99); //99 is Zelda
		if (tmp)
		{

			int percent_life = (tmp->life * 100) / 20;
			ChangeColor(percent_life);

			//magic numbers :c
			int image_width = 280;
			int move_to = image_width - ((percent_life * image_width) / 100);
			move_rect.x = draw_rect.x - move_to;
			return;
		}
		
	}
	
}

void UI_Stamina::ChangeColor(int percent)
{
	if (element_type == STAMINA_BAR)
	{
		if (percent < 40)
		{
			if (percent < 15)
			{
				SetStamina(App->game->hud->stamina_red);
				return;
			}
			SetStamina(App->game->hud->stamina_yellow);
			return;
		}

		if (stamina != App->game->hud->stamina_green)
			SetStamina(App->game->hud->stamina_green);

		return;
	}

	if (element_type == ZELDA_LIFE)
	{
		if (percent < 60)
		{
			if (percent < 30)
			{
				SetStamina(App->game->hud->low_life);
				return;
			}
			SetStamina(App->game->hud->mid_life);
			return;
		}

		if (stamina != App->game->hud->full_life)
			SetStamina(App->game->hud->full_life);

		return;
	}

}

void UI_Stamina::PrintBar()
{
	int difference_x = draw_rect.x - move_rect.x;
	SDL_Rect tmp = { stamina->Image.x + difference_x, stamina->Image.y,stamina->Image.w, stamina->Image.h };
	App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), ((Parent->Interactive_box.x + draw_rect.x) - App->render->camera.x) * App->gui->scale_factor, ((Parent->Interactive_box.y + draw_rect.y) - App->render->camera.y) * App->gui->scale_factor, &tmp);
}

