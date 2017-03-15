#ifndef __TEST_SCENE_H__
#define __TEST_SCENE_H__

#include "Scene.h"

class TestScene : public Scene
{
public:

	TestScene();
	bool Load(pugi::xml_node& config);
	bool Update(float dt);

public:

	Room* testRoom;
	pugi::xml_node data;
};

#endif // __TEST_SCENE_H__