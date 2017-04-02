#include "TestScene.h"
#include "Scene.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1SceneManager.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Render.h"

TestScene::TestScene() {}

bool TestScene::Load(std::string _data)
{
	bool ret = true;

	data = _data;

	if (ret = (App->map->Load("test.tmx")))
	{
		int w, h;
		uchar* player_data = NULL;
		uchar* enemy_data = NULL;

		if (App->map->CreateWalkabilityMap(w, h, &player_data, &enemy_data))
		{
			App->pathfinding->SetPlayerMap(w, h, player_data);
			App->pathfinding->SetEnemyMap(w, h, enemy_data);
		}

		RELEASE_ARRAY(player_data);
		RELEASE_ARRAY(enemy_data);
	}

	return ret;
}

// Called each loop iteration
bool TestScene::Update(float dt)
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
	{
		/*if (testRoom == NULL)
		{
			testRoom = new Room("tmpRoom");
			testRoom->Load("data");
		}
		else
		{
			testRoom->CleanUp();
			RELEASE(testRoom);
			testRoom = NULL;
		}*/
	}
	else
	{
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
			ret = false;
	}
	
	App->map->Draw();

	return ret;
}
