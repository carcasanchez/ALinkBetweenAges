#ifndef __TEST_SCENE_H__
#define __TEST_SCENE_H__

#include "Scene.h"

class TestScene : public Scene
{
public:

	TestScene();
	bool Start();
	bool Update(float dt);
};

#endif // __TEST_SCENE_H__