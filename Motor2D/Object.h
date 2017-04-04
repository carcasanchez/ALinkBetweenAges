#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Entity.h"
#include "InputManager.h"
#include "j1PerfTimer.h"



class Object : public Entity
{
public:

	Object() {};
	bool Spawn(std::string file, iPoint pos);
	void OnDeath();


public:

};

#endif // !_OBJECT_H_