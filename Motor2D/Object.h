#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Entity.h"
#include "j1EntityManager.h"
#include "InputManager.h"
#include "j1PerfTimer.h"
#include "j1GameLayer.h"


enum OBJECT_TYPE
{
	NO_OBJECT,
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
	BOMB,
	HEART_CONTAINER,
	ARROW_DROP,
	BOMB_DROP,
	LIFE_POTION,
	BOMB_SAC,
	ZELDA_ARROW,
	SWORD_BOLT,
	FALLING_BOLT,
	BOMB_EXPLOSION,
	BOW,
	STAMINA_POTION,
	PILLAR,
	STONE,
	BOSS_KEY,
	INTERRUPTOR
};

class Object : public Entity
{
public:

	Object() {};
	bool Spawn(std::string file, iPoint pos, OBJECT_TYPE type);
	void OnDeath();

	virtual bool Update(float dt) 
	{ 
		return true; 
	};

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


class ZeldaArrow : public Object
{
public:

	ZeldaArrow() {};

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

	Bomb() 
	{
		explode_counter.Start();
	};
	bool Update(float dt) 
	{ 

		if (explode_counter.ReadMs() > explode_time)
			ExplodeBomb(); 

		return true;
	};
	
	bool ExplodeBomb();

public:
	int explode_time = 2000;
	j1PerfTimer explode_counter;
};

class BombExplosion : public Object
{
public:

	BombExplosion()
	{
		
	};

	bool Update(float dt)
	{
		return DeleteExplode();
	};

	bool DeleteExplode();
public:
	int dead_time = 1000;
	j1PerfTimer dead_counter;
};

class FallingBolt : public Object
{
public:

	FallingBolt() {};
	bool Update(float dt);
};

class Bow : public Object
{
public:

	Bow() {};
};

class StaminaPotion : public Object
{
public:

	StaminaPotion() {};
};

class Pillar : public Object
{
public:

	Pillar() {};

	bool Update(float dt);
	
};

class Interruptor : public Object
{
public:

	Interruptor() {};

	bool on = false;


};


#endif // !_OBJECT_H_