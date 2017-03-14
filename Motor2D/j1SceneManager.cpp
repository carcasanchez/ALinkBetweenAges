#include "j1SceneManager.h"
#include "Scene.h"
#include "TestScene.h"

j1SceneManager::j1SceneManager() : currentScene(NULL)
{

}

j1SceneManager::~j1SceneManager()
{

}

bool j1SceneManager::Awake(pugi::xml_node& config)
{
	return true;
}

bool j1SceneManager::Start()
{
	currentScene = new TestScene();
	return currentScene->Start();
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