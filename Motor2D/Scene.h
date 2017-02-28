#ifndef __SCENE_H__
#define __SCENE_H__

#include "PugiXml\src\pugixml.hpp"
#include <string>

class UI_element;
enum GUI_INPUT;

using namespace std;

class Scene
{
public:
	Scene() : name("unnamed scene") {}
	Scene(string name){}
	virtual ~Scene(){}
	virtual bool Initialize(pugi::xml_node& config) { return true; }
	virtual bool Start() { return true; }
	virtual bool PreUpdate() { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate() { return true; }
	virtual bool CleanUp() { return true; }

	virtual bool On_GUI_Callback(UI_element* elem, GUI_INPUT gui_input) { return true; }

public:

	string name;

};






#endif // __SCENE_H__