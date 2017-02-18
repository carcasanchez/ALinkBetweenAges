#include "j1EntityManager.h"
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
	Entity_textures = App->tex->Load(Entity_texture_name.c_str());

	for (std::list<entity*>::iterator item = Entities.begin(); item != Entities.end(); item++)
		(*item)->Start();

	
	return true;
}

bool j1EntityManager::PreUpdate()
{

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
		Entity_selected();

	if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE))
		Entity_disselected();

	for (std::list<entity*>::iterator item = Entities.begin(); item != Entities.end(); item++)
		(*item)->PreU();
			
	return true;
}

bool j1EntityManager::Update(float dt)
{
	
	for (std::list<entity*>::iterator item = Entities.begin(); item != Entities.end(); item++)
		(*item)->U(dt);
		
	return true;
}

bool j1EntityManager::UpdateTicks()
{
	
	for (std::list<entity*>::iterator item = Entities.begin(); item != Entities.end(); item++)
		(*item)->UTicks();
	return true;
}

bool j1EntityManager::PostUpdate()
{
	for (std::list<entity*>::iterator item = Entities.begin(); item != Entities.end(); item++)
		(*item)->PostU();

	return true;
}

bool j1EntityManager::CleanUp()
{

	for (std::list<entity*>::iterator item = Entities.begin(); item != Entities.end(); item++)
		Entities.erase(item);

	Entities.clear();
	
	return true;
}

entity* j1EntityManager::create(p2Point<int> position)
{	
	entity* ent = new Player(position);

	if(ent)
		Entities.push_back(ent);

	return ent;
}

void j1EntityManager::Entity_selected()
{

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMapMouse(p.x, p.y);

	LOG("mouse right %i %i", p.x, p.y);
	
	for (std::list<entity*>::iterator item = Entities.begin(); item != Entities.end(); item++)
	{
		if (p == App->map->WorldToMap((*item)->position.x, (*item)->position.y))
			(*item)->is_Selected();
	}
	
}

void j1EntityManager::Entity_disselected()
{
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMapMouse(p.x, p.y);

	LOG("mouse right %i %i", p.x, p.y);

	for (std::list<entity*>::iterator item = Entities.begin(); item != Entities.end(); item++)
	{
		if (p == App->map->WorldToMap((*item)->position.x, (*item)->position.y))
			(*item)->not_Selected();
	}
}