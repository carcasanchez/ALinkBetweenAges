#ifndef __SCENE_H__
#define __SCENE_H__

#include "PugiXml\src\pugixml.hpp"
#include <string>

//class UI_element;
//enum GUI_INPUT;

class Scene
{

public:

	Scene();
	Scene(const char*);
	virtual ~Scene() {}
	virtual bool Load(const char*);
	virtual bool Update(float);
	virtual bool CleanUp();

	//virtual bool On_GUI_Callback(UI_element* elem, GUI_INPUT gui_input) { return true; }

public:

	std::string name;
	int currentSector;
	int maxSectors;
};

#endif // __SCENE_H__