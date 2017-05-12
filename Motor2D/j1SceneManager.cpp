#include "j1SceneManager.h"
#include "j1App.h"
#include "j1GameLayer.h"
#include "j1EntityManager.h"
#include "Entity.h"
#include "Player.h"
#include "Scene.h"
#include "j1CutSceneManager.h"
#include "TestScene.h"
#include "j1Map.h"
#include "j1CollisionManager.h"
#include "j1Pathfinding.h"
#include "j1Audio.h"


j1SceneManager::j1SceneManager() : currentScene(nullptr)
{
	name = ("sceneManager");
}

j1SceneManager::~j1SceneManager()
{}

bool j1SceneManager::Awake(pugi::xml_node& config)
{
	pugi::xml_node scenes = config.child("scenes");
	std::string folder = scenes.attribute("folder").as_string("/");
	pugi::xml_node scene = scenes.first_child();

	while (!scene.empty())
	{
		std::string name = scene.attribute("name").as_string("Unnamed Scene");
		data.insert(std::pair<std::string, std::string >(name, (folder + scene.attribute("file").as_string(".xml"))));
		scene = scene.next_sibling();
	}

	return !data.empty();
}

bool j1SceneManager::Start()
{	

	currentScene = new Scene("introScene");
	currentScene->Load(data[currentScene->name].c_str());
	
 	App->cutsceneM->StartCutscene(0);

	return true;
}

bool j1SceneManager::Update(float dt)
{
	bool ret = true;

	if (!changeRequest)
	{
		ret = currentScene->Update(dt);
	}
	else
	{
		ChangeScene();
	}


	return ret;
}

bool j1SceneManager::CleanUp()
{
	currentScene->CleanUp();
	RELEASE(currentScene);
	return true;
}

void j1SceneManager::RequestSceneChange(Exit* exit)
{
	App->game->em->player->sceneOverride = true;
	changeRequest = true;

	destiny = exit->destiny;
	exitDest = exit->exitDest;

	

	// Apply transition
}

void j1SceneManager::RequestSceneChange(iPoint dest, const char * scene, DIRECTION dir)
{
	if(App->game->em->player)
		App->game->em->player->sceneOverride = true;
	
	changeRequest = true;

	destiny = scene;
	exitDest = -1;
	this->dir = dir;
	spawnPoint = dest;
}

bool j1SceneManager::ChangeScene()
{
	bool ret = true;

	ret = currentScene->CleanUp();
	App->map->CleanUp();
	

	if (ret)
	{

		App->pathfinding->CleanUp();
		//App->collisions->ChangeScene();
		App->game->em->CleanEntities();
		RELEASE(currentScene);
		currentScene = new Scene(destiny.c_str());
		currentScene->Load(data[currentScene->name].c_str());
		

		iPoint destPos;
		if (exitDest != -1)
		{
				 destPos = currentScene->GetExitPlayerPos(1, exitDest, dir);
			switch (dir)
			{
			case(D_UP):
				destPos.y -= 16;
				break;
			case(D_DOWN):
				destPos.y += 16;
				break;
			case(D_RIGHT):
				destPos.x += 16;
				break;
			case(D_LEFT):
				destPos.x -= 16;
				break;
			}

			App->SaveGame("saves.xml");

		}
		else
		{
			destPos = spawnPoint;
		}
		

		App->game->em->player->currentPos = destPos;
		App->game->em->player->currentDir = dir;
		changeRequest = false;
		
	}

	App->render->CenterCamera(App->game->em->player->currentPos);

	//temporal
	App->game->em->player->sceneOverride = false;

	return ret;
}

