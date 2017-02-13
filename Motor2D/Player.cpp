#include "Player.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Map.h"


Player::Player(p2Point<int> pos) : entity(App->map->MapToWorld(pos.x,pos.y))
{
	speed = 10;
};

bool Player::Start()
{
	selectionable = true;


	return true;
}

bool Player::PreU()
{
	
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMapMouse(p.x, p.y);
	
	if (path_length <= 0 && selected)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && on_path == false)
		{
			LOG("mouse %i %i", p.x, p.y);

			path_length = App->pathfinding->CreatePath(App->map->WorldToMap(position.x, position.y), p);

			if (path_length > 0)
			{
				for (int i = 0; i < App->pathfinding->GetLastPath()->Count(); i++)
					path.add(*App->pathfinding->GetLastPath()->At(i));

				on_path = true;
			}
		}

	}
	
	return true;
}

bool Player::UTicks()
{
	if (on_path)
	{

		if (App->map->WorldToMap(position.x, position.y) == path.At(path_length)->data)
			on_path = false;

		else
		{
			position = App->map->MapToWorld(path.At(last_point)->data.x, path.At(last_point)->data.y);
			last_point++;
		}
	}
	

	return true;
}

bool Player::U(float dt)
{
	

	App->render->Blit(App->entity_manager->Entity_textures, position.x, position.y, &player_text);
	return true;
}

bool Player::PostU()
{
	if (!on_path && path_length > 0)
	{
		last_point = 0;
		path_length = 0;
		path.clear();
	}

	return true;
}
