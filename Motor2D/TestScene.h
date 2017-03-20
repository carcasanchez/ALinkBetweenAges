#ifndef __TEST_SCENE_H__
#define __TEST_SCENE_H__

#include "Scene.h"

class TestScene : public Scene
{
public:

	TestScene();
	bool Load(std::string data);
	bool Update(float dt);

public:

	std::string data;
};

#endif // __TEST_SCENE_H__