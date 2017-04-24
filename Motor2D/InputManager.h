#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include "j1Module.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <map>
#include <string>

enum GAMECONTEXT
{
	IN_GAME,
	IN_MENU
};

enum INPUTEVENT
{
	NO_EVENT = -1,
	ATTACK_UP = 0,
	ATTACK_DOWN,
	ATTACK_LEFT,
	ATTACK_RIGHT,
	DODGE,
	PAUSE,

	//Inside Menus
	UP,
	DOWN,
	LEFT,
	RIGHT,
	CONFIRM,
	DECLINE,

	//No Button Asociated -> Joysticks
	MUP,
	MDOWN,
	MLEFT,
	MRIGHT,
	LOOKUP,
	LOOKDOWN,
	LOOKLEFT,
	LOOKRIGHT,
	SPIN_TO_WIN,
	NOT_TO_SPIN,
	USE_ITEM,
	STOP_ITEM
	
};

enum EVENTSTATE
{
	E_NOTHING,
	E_DOWN,
	E_UP,
	E_REPEAT
};


enum JSTATE
{
	J_NONE,
	J_POSITIVE,
	J_NEGATIVE
};

class InputListener
{
public:
	InputListener() : input_active(false) {}
	bool input_active;
	virtual void OnInputCallback(INPUTEVENT, EVENTSTATE) {};
};

class InputManager : public j1Module
{
public:

	InputManager();

	// Destructor
	virtual ~InputManager();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Called before all Updates
	bool PreUpdate();

	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//When detected input
	void InputDetected(int, EVENTSTATE);
	void JoystickDetected(int, JSTATE);

	//To Change the action button
	void ChangeInputEvent(INPUTEVENT);

	//For Polling
	EVENTSTATE EventPressed(INPUTEVENT) const;

	//For Callback system
	void AddListener(InputListener*);
	void CallListeners();

	//To change the game context
	GAMECONTEXT GetGameContext();
	void SetGameContext(GAMECONTEXT);

private:

	//Mapping is fun
	//All the actions possible int->button, INPUTEVENT->attack, moveup...
	std::map<GAMECONTEXT, std::multimap<int, INPUTEVENT>> context_filter;

	//All the actions in this frame
	std::multimap<INPUTEVENT, EVENTSTATE> current_action;

	//All listeners for the callbacks
	std::list<InputListener*> listeners;

	//GAMECONTEXT
	GAMECONTEXT  actions_filter;

	//To Change the action button
	bool		next_input_change = false;
	bool		ChangeEventButton(int);
	INPUTEVENT	event_to_change = NO_EVENT;


};

#endif // __INPUT_MANAGER_H__