#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Entity.h"
#include "InputManager.h"
#include "j1PerfTimer.h"

enum OBJECT_TYPE
{
	BOOK = 0,
	ARROW
};

class Object : public Entity
{
public:

	Object() {};
	bool Spawn(std::string file, iPoint pos, OBJECT_TYPE type);
	void OnDeath();


public:
	OBJECT_TYPE objectType = BOOK;

};

#endif // !_OBJECT_H_