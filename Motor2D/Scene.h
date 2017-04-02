#ifndef __SCENE_H__
#define __SCENE_H__

#include "PugiXml\src\pugixml.hpp"
#include "p2Point.h"
#include <string>
#include <map>
#include <list>

//class UI_element;
//enum GUI_INPUT;

class Exit;
enum DIRECTION;

class Scene
{

public:

	Scene();
	Scene(const char*);
	virtual ~Scene() {}
	virtual bool Load(const char*, const bool reloadMap = true);
	virtual bool Update(float);
	virtual bool CleanUp();

	//virtual bool On_GUI_Callback(UI_element* elem, GUI_INPUT gui_input) { return true; }

	iPoint GetExitPlayerPos(int, int);

public:

	std::string name;
	int currentSector;
	int maxSectors;

private:

	std::map<int, std::list<Exit*>> exits;
};

#endif // __SCENE_H__