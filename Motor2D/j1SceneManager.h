#ifndef _SCENE_MANAGER_H_
#define _SCENE_MANAGER_H_

#include "j1Module.h"
#include <map>

class Scene;

class j1SceneManager : public j1Module
{

public:

	j1SceneManager();
	~j1SceneManager();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool CleanUp();

public:
	Scene* currentScene;

	std::map<std::string, std::string> dir;
};



#endif // !_SCENE_MANAGER_H_