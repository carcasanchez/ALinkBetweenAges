#include "UI_Counter.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Render.h"

UI_Counter::UI_Counter(UI_TYPE typ, j1Module* callback) : UI_element(typ, callback) {}

bool UI_Counter::Update_Draw()
{

	BlitUnit();
	BlitDec();
	BlitCent();

	return true;
}

void UI_Counter::SumNumber(int plus)
{
	int full_number = (cent * 100) + (dec * 10) + unit;
	
	if ((full_number + plus) < 999)
	{
		full_number += plus;

		cent = full_number / 100;
		dec = (full_number % 100) / 10;
		unit = ((full_number % 100) % 10);
	}
	else
	{
		cent = 9;
		dec = 9;
		unit = 9;
	}
}

void UI_Counter::QuitNumber(int less)
{
	int full_number = (cent * 100) + (dec * 10) + unit;

	if ((full_number - less) >= 0)
	{
		full_number -= less;
		cent = full_number / 100;
		dec = (full_number % 100) / 10;
		unit = ((full_number % 100) % 10);
	}
	else
	{
		cent = 0;
		dec = 0;
		unit = 0;
	}
}

void UI_Counter::SetImage(UI_Image* new_img, int width, int height)
{
	if(new_img)
		img = new_img;

	this->width = width;
	this->height = height;

	if(width && height)
	{
		columns = img->Image.w / width;
		rows = img->Image.h / height;
	}

}

void UI_Counter::Reset()
{
	unit = 0;
	dec = 0;
	cent = 0;
}

void UI_Counter::BlitUnit()
{
	if (Parent)
	{
		int pos_x = 0;
		int pos_y = 0;
		SDL_Rect tmp = {0,0,0,0};

		if (unit >= columns)
		{
			pos_x = unit - columns;
			pos_y++;
		}
		else pos_x = unit;
		
		if (img)
			tmp = { img->Image.x + (pos_x * width),  img->Image.y + (pos_y * height), width, height };


		App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), ((Parent->Interactive_box.x + Interactive_box.x + (width * 4)) - App->render->camera.x) * App->gui->scale_factor, ((Parent->Interactive_box.y + Interactive_box.y) - App->render->camera.y) * App->gui->scale_factor, &tmp);
	}

}

void UI_Counter::BlitDec()
{
	if (Parent)
	{
		int pos_x = 0;
		int pos_y = 0;
		SDL_Rect tmp = { 0,0,0,0 };

		if (dec >= columns)
		{
			pos_x = dec - columns;
			pos_y++;
		}
		else pos_x = dec;

		if (img)
			tmp = { img->Image.x + (pos_x * width),  img->Image.y + (pos_y * height), width, height };


		App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), ((Parent->Interactive_box.x + Interactive_box.x + (width * 2)) - App->render->camera.x) * App->gui->scale_factor, ((Parent->Interactive_box.y + Interactive_box.y) - App->render->camera.y) * App->gui->scale_factor, &tmp);
	}
}

void UI_Counter::BlitCent()
{
	if (Parent)
	{
		int pos_x = 0;
		int pos_y = 0;
		SDL_Rect tmp = { 0,0,0,0 };

		if (cent >= columns)
		{
			pos_x = cent - columns;
			pos_y++;
		}
		else pos_x = cent;

		if (img)
			tmp = { img->Image.x + (pos_x * width),  img->Image.y + (pos_y * height), width, height };


		App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), ((Parent->Interactive_box.x + Interactive_box.x) - App->render->camera.x) * App->gui->scale_factor, ((Parent->Interactive_box.y + Interactive_box.y) - App->render->camera.y) * App->gui->scale_factor, &tmp);
	}
}

