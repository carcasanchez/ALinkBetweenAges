#ifndef _SCENE_MANAGER_H_
#define _SCENE_MANAGER_H_

#include "j1Module.h"
#include "p2Point.h"
#include <map>

/*enum SCENE_NAME
{
	TEST = -1,
	GENERIC,
	MAIN_MENU,
	KAKARIKO,
	FOREST,
	GREENZONE1,
	GREENZONE2,
	WITCHPLACE,
	WITCHHOUSE,
	DESERT1,
	DESERT2,
	DUNES,
	TEMPLE,
	DUNGEON,
	MASTERSWORD,
	CASTLEOUT,
	CASTLEIN,
	BOSS
};*/

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

	void RequestSceneChange(std::string, iPoint);
	bool ChangeScene();

public:
	Scene* currentScene;

	std::map<std::string, std::string> data;

	bool changeRequest = false;
	std::string destiny;
	iPoint destPos;
};



#endif // !_SCENE_MANAGER_H_