#ifndef __EXIT_H__
#define __EXIT_H__

#include "p2Point.h"
#include "SDL\include\SDL_rect.h"
#include <string>

class Collider;
class Sprite;
enum DIRECTION;

class Exit
{
public:

	Exit();
	~Exit();

	bool Spawn(std::string, int, SDL_Rect, DIRECTION);
	void Update();
	void Draw();
	bool CleanUp();

public:

	std::string destiny;
	int exitDest;
	SDL_Rect rect;
	DIRECTION dir;

	bool locked;

	// Graphic Resources
	Sprite* sprite;

	// Collider
	Collider* col;
};


#endif // __EXIT_H__