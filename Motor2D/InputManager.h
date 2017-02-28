#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include "j1Module.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <string>

enum INPUT_TYPE
{
	DOWN,
	UP,
	REPEAT
};

struct ShortCut
{
	ShortCut() : active(false)
	{}

	ShortCut(INPUT_TYPE _type, string _name, string _command) : active(false)
	{
		type = _type;
		name = _name;
		command = _command;
	}

	INPUT_TYPE	 type;
	bool		 active = false;
	bool		 ready_to_change = false;
	std::string		 name;
	std::string		 command; // SDL_ScancodeName
};

enum ShortCutID
{
	A = 0,
	B,
	X,
	Y,
	RightTrigger,
	LeftTrigger,
	RightBumper,
	LeftBumper
};

/*-Left stick : move link(8 directions)
- Right Stick : orient link(4 directions).If in neutral position, Link will orient automatically with left stick.
- Right Bumper : Attack in the current orientation if rapid touch, spin attack if hold an release.
- Left Bumper : Dodge in the current orientation.If neutral, dodge backwards.
- Right Trigger : Use equipped item.
- Left Trigger : Change item.
- A : Interact(pick objects, talk), acept in menu.
- B : Cancel in menu.
- Y : Open map.
- Start : Open pause menu.*/

class InputManager: public j1Module
{
public:

	InputManager();

	// Destructor
	virtual ~InputManager();

	// Called when before render is available
	bool awake(pugi::xml_node&);

	// Called before all Updates
	bool preUpdate();

	bool update(float dt);

	// Called after all Updates
	bool postUpdate();

	// Called before quitting
	bool cleanUp();

	//Shortcuts list
	std::list<ShortCut*>	shortcuts_list;

	//Check for shortcut state
	bool CheckShortcut(ShortCutID id = A);

private:

	//Refresh commands once have been changed
	void ChangeShortcutCommand(ShortCut* shortcut);

	//Shortcuts xml file path
	std::string		inputs_file_path;
};

#endif // __INPUT_MANAGER_H__