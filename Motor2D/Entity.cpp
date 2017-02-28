#include "Entity.h"
#include "j1Input.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Render.h"

Entity::Entity() {};

Entity::Entity(p2Point<int> pos) : position(pos) {};

void Entity::is_Selected()
{
	if (selectionable)
		 selected = true;	
}

void Entity::not_Selected()
{
	if (selected)
		selected = false;
}

void Entity::Set_texture(SDL_Rect new_img)
{
	player_text = new_img;
}
