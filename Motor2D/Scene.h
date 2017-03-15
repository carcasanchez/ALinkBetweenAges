#ifndef __SCENE_H__
#define __SCENE_H__

#include "PugiXml\src\pugixml.hpp"
#include <string>

class UI_element;
enum GUI_INPUT;
class Room;

/*
THIS IS NOT A MODULE BECAUSE I NEEDS MORE FUNCTIONALITY
THAN THAT OF A MODULE

PLS DONT CHANGE
*/

class Scene
{

public:

	Scene() : name("unnamed scene") {}
	Scene(std::string name) : name(name) {}
	virtual ~Scene() {}
	virtual bool Load(std::string data);
	virtual bool PreUpdate() { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate() { return true; }
	virtual bool CleanUp() { return true; }

	virtual bool On_GUI_Callback(UI_element* elem, GUI_INPUT gui_input) { return true; }

public:

	std::string name;
};



class Room : public Scene
{

public:

	Room();
	Room(std::string name) : Scene(name) {}
	~Room() {}

	virtual bool Load(std::string data);
	virtual bool CleanUp();
};




#endif // __SCENE_H__