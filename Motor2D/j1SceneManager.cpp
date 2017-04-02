#include "j1SceneManager.h"
#include "Scene.h"
#include "TestScene.h"
#include "j1EntityManager.h"
#include "p2Defs.h"

j1SceneManager::j1SceneManager() : currentScene(NULL)
{
	name = ("sceneManager");
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
		//std::pair<Scene*, std::string> pair = { NULL, (folder + scene.attribute("file").as_string(".xml")) };

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
	return currentScene->Update(dt);
}

bool j1SceneManager::CleanUp()
{
	RELEASE(currentScene);
	return true;
}

bool j1SceneManager::ChangeScene(std::string newScene)
{
	bool ret = true;

	ret = currentScene->CleanUp();

	if (ret)
	{
		ret = currentScene->Load(data[newScene].c_str());
	}

	return ret;
}