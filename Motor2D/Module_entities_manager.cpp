#include "Module_entites_manager.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1App.h"
#include "j1Textures.h"
#include "p2Log.h"
#include "j1Render.h"
#include "Player.h"



j1EntityManager::j1EntityManager()
{
	name = ("entity_manager");
}

bool j1EntityManager::Awake(pugi::xml_node & config)
{
	LOG("Loading Entity textures");

	Entity_texture_name = config.child("entities_texture").attribute("file").as_string("");


	return true;
}

bool j1EntityManager::Start()
{
	Entity_textures = App->tex->Load(Entity_texture_name.GetString());

	for (int i = 0; i < Entities.count(); i++)
		Entities[i]->Start();

	return true;
}

bool j1EntityManager::PreUpdate()
{

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
		Entity_selected();

	if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE))
		Entity_disselected();

	for (int i = 0; i < Entities.count(); i++)
		Entities[i]->PreU();
			
	return true;
}

bool j1EntityManager::Update(float dt)
{
	

	for (int i = 0; i < Entities.count(); i++)
		Entities[i]->U(dt);
		
	return true;
}

bool j1EntityManager::UpdateTicks()
{
	
	for (int i = 0; i < Entities.count(); i++)
		Entities[i]->UTicks();
	return true;
}

bool j1EntityManager::PostUpdate()
{
	for (int i = 0; i < Entities.count(); i++)
		Entities[i]->PostU();

	return true;
}

bool j1EntityManager::CleanUp()
{
	Entities.clear();
	
	return true;
}

entity* j1EntityManager::create(p2Point<int> position)
{	
	entity* ent = new Player(position);

	if(ent)
		Entities.add(ent);

	return ent;
}

void j1EntityManager::Entity_selected()
{

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMapMouse(p.x, p.y);

	LOG("mouse right %i %i", p.x, p.y);
	
	for (int i = 0; i < Entities.count(); i++)
	{
		if (p == App->map->WorldToMap(Entities[i]->position.x, Entities[i]->position.y))
			Entities[i]->is_Selected();
	}

	
	
}

void j1EntityManager::Entity_disselected()
{
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMapMouse(p.x, p.y);

	LOG("mouse right %i %i", p.x, p.y);

	for (int i = 0; i < Entities.count(); i++)
	{
		if (p == App->map->WorldToMap(Entities[i]->position.x, Entities[i]->position.y))
			Entities[i]->not_Selected();
	}
}