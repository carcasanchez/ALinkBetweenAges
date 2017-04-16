#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Entity.h"
#include "InputManager.h"
#include "j1PerfTimer.h"

enum OBJECT_TYPE
{
	BOOK = 0,
	LINK_ARROW
};

class Object : public Entity
{
public:

	Object() {};
	bool Spawn(std::string file, iPoint pos, OBJECT_TYPE type);
	void OnDeath();

	virtual bool Update(float dt) { return true; };

public:
	OBJECT_TYPE objectType = BOOK;

};

class Arrow : public Object
{
public:

	Arrow() {};

	

	bool Update(float dt)
	{ 
		bool ret;
		speed = 500;
		switch (currentDir)
		{
		case D_UP:
			ret = Move(0, -SDL_ceil(speed * dt));
			break;
		case D_DOWN:
			ret = Move(0, SDL_ceil(speed * dt));
			break;
		case D_LEFT:
			ret = Move(-SDL_ceil(speed * dt), 0);
			break;
		case D_RIGHT:
			ret = Move(SDL_ceil(speed * dt), 0);
			break;
		}

		if (!ret)
		{
			life = -1;
		}

		return ret;
	};

public:

};


#endif // !_OBJECT_H_