// ----------------------------------------------------
// j1Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __j1MODULE_H__
#define __j1MODULE_H__

#include <string>
#include <list>
#include <vector>
using namespace std;

#include "PugiXml\src\pugixml.hpp"

class j1App;
struct UI_element;
enum GUI_INPUT;
struct command;
class CVar;
class Collider;


class j1Module
{
public:

	j1Module() : active(false)
	{}

	virtual ~j1Module()
	{}

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	virtual bool UpdateTicks()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	virtual bool On_GUI_Callback(UI_element*, GUI_INPUT) { return true; }
	//THHIS TWO ARE THRASH
	virtual bool On_Console_Callback(command*, int*) { return true; };
	virtual bool On_Console_Callback(command*, char*) { return true; };


	virtual bool On_Console_Callback(CVar*) { return true; };
	virtual bool On_Console_Callback(command*) { return true; };
	virtual bool On_Collision_Callback(Collider*, Collider*, float dt) { return true; };

public:

	string		name;
	bool		active;

};

#endif // __j1MODULE_H__