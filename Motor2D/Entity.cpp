#include "Entity.h"
#include "j1Input.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Render.h"

entity::entity(p2Point<int> pos) : position(pos) {};

void entity::is_Selected()
{
	if (selectionable)
		 selected = true;	
}

void entity::not_Selected()
{
	if (selected)
		selected = false;
}

void entity::Set_texture(SDL_Rect new_img)
{
	player_text = new_img;
}
