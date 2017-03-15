#include "j1SceneManager.h"
#include "Scene.h"
#include "TestScene.h"

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
		dir.insert(std::pair<std::string, std::string >(scene.attribute("name").as_string("Unnamed Scene"), (folder + scene.attribute("file").as_string(".xml"))));
		scene = scene.next_sibling();
	}

	return true;
}

bool j1SceneManager::Start()
{
	currentScene = new TestScene();
	return currentScene->Load(dir[currentScene->name]);
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