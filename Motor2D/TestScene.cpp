#include "TestScene.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1PathFinding.h"

TestScene::TestScene() : Scene("Test Scene") {}

bool TestScene::Start()
{
	bool ret = true;

	if (ret = (App->map->Load("map_testing.tmx")))
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	return ret;
}

// Called each loop iteration
bool TestScene::Update(float dt)
{
	bool ret = true;

	App->map->Draw();

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	
	return ret;
}
