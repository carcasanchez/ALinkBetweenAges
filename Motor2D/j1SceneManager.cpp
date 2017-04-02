#include "j1SceneManager.h"
#include "j1App.h"
#include "j1GameLayer.h"
#include "j1EntityManager.h"
#include "Entity.h"
#include "Player.h"
#include "Scene.h"
#include "TestScene.h"

j1SceneManager::j1SceneManager() : currentScene(NULL)
{
	name = ("sceneManager");
	destPos = { 0, 0 };
}

j1SceneManager::~j1SceneManager()
{

}

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
	currentScene = new Scene("insideCastle");
	return currentScene->Load(data[currentScene->name].c_str());
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
	RELEASE(currentScene);
	return true;
}

void j1SceneManager::RequestSceneChange(std::string dest, iPoint pos)
{
	App->game->em->player->sceneOverride = true;

	changeRequest = true;
	destiny = dest;
	destPos = pos;

	// Apply transition
}

bool j1SceneManager::ChangeScene()
{
	bool ret = true;

	ret = currentScene->CleanUp();

	if (ret)
	{
		App->game->em->CleanEntities();
		ret = currentScene->Load("scenes/kakariko.xml", false);

		App->game->em->player->currentPos = destPos;

		changeRequest = false;
	}

	//temporal
	App->game->em->player->sceneOverride = false;

	return ret;
}