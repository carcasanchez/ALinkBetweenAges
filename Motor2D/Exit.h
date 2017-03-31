#ifndef __EXIT_H__
#define __EXIT_H__

#include "p2Point.h"
#include <string>

class Collider;
class Sprite;
enum DIRECTION;

class Exit
{
public:

	Exit();
	~Exit();

	bool Spawn(DIRECTION, iPoint, std::string, iPoint);
	void Update();
	void Draw();
	bool CleanUp();

public:

	iPoint currentPos;
	DIRECTION dir;
	bool locked;
	std::string destiny;
	iPoint destPos;

	// Graphic Resources
	Sprite* sprite;

	// Collider
	Collider* col;
	iPoint colPivot;
};


#endif // __EXIT_H__