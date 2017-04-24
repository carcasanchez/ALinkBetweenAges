#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Entity.h"
#include "InputManager.h"
#include "j1PerfTimer.h"

enum OBJECT_TYPE
{
	BOOK = 0,
	LINK_ARROW,
	GREEN_RUPEE,
	BLUE_RUPEE,
	RED_RUPEE,
	LIFEHEART,
	BUSH,
	POT,
	OCTO_STONE,
	CHEST,
	MAGIC_SLASH,
	BOMB
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
	int price = 0;
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

class Rupee : public Object
{
public:

	Rupee() {};
	
public:
	int rupeeValue = 0;
};

class Octostone :public Object
{
public:
	Octostone() {};

public:
	
	bool Update(float dt)
	{
		bool ret;
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
};

class Chest : public Object
{
public:

	Chest() {};

public:
	OBJECT_TYPE objectInside = GREEN_RUPEE;
};


class MagicSlash : public Object
{
public:

	MagicSlash() {};

	bool Update(float dt)
	{
		bool ret;
		switch (currentDir)
		{
		case D_UP:
			//currentPos.y -= SDL_ceil(speed * dt);
			ret = Move(0, -SDL_ceil(speed * dt));
			break;
		case D_DOWN:
			ret = Move(0, SDL_ceil(speed * dt));
		//	currentPos.y += SDL_ceil(speed * dt);		
			break;
		case D_LEFT:
			ret = Move(-SDL_ceil(speed * dt), 0);
			//currentPos.x -= SDL_ceil(speed * dt);			
			break;
		case D_RIGHT:
			ret = Move(SDL_ceil(speed * dt), 0);
			//currentPos.x += SDL_ceil(speed * dt);
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


class Bomb : public Object
{
public:

	Bomb() {};

public:
	int timer;
};


#endif // !_OBJECT_H_