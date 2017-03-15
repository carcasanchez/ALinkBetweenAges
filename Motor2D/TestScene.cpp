#include "TestScene.h"
#include "Scene.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1SceneManager.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1PathFinding.h"

TestScene::TestScene() : Scene("Test Scene"), testRoom(NULL) {}

bool TestScene::Load(std::string _data)
{
	bool ret = true;

	data = _data;

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

	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
	{
		if (testRoom == NULL)
		{
			testRoom = new Room("tmpRoom");
			testRoom->Load("data");
		}
		else
		{
			testRoom->CleanUp();
			RELEASE(testRoom);
			testRoom = NULL;
		}
	}
	else
	{
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
			ret = false;
	}
	
	App->map->Draw();

	return ret;
}
