#include "InputManager.h"
#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1FileSystem.h"
#include "j1Input.h"
#include "j1Fonts.h"
#include "j1Audio.h"
#include "SDL\include\SDL.h"

InputManager::InputManager() : j1Module()
{
	name = "inputManager";
}

// Destructor
InputManager::~InputManager()
{}

// Called when before render is available
bool InputManager::Awake(pugi::xml_node& conf)
{
	bool ret = true;

	//Load All actions
	
	for (pugi::xml_node con_tmp = conf.child("context"); con_tmp != nullptr; con_tmp = con_tmp.next_sibling())
	{
		std::pair<GAMECONTEXT, std::multimap<int, INPUTEVENT>> new_context;
		new_context.first = (GAMECONTEXT)con_tmp.attribute("value").as_int();

		for (pugi::xml_node act_tmp = con_tmp.child("action"); act_tmp != nullptr; act_tmp = act_tmp.next_sibling())
		{
			std::pair<int, INPUTEVENT> new_action;
			new_action.first = act_tmp.attribute("button").as_int();
			new_action.second = (INPUTEVENT)act_tmp.attribute("event").as_int();

			new_context.second.insert(new_action);
		}

		context_filter.insert(new_context);
	}

	actions_filter = IN_GAME;

	return ret;
}

// Called before all Updates
bool InputManager::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool InputManager::Update(float dt)
{


	




	return true;
}

// Called after all Updates
bool InputManager::PostUpdate()
{
	CallListeners();
	if (!current_action.empty())
		current_action.clear();

	return true;
}

// Called before quitting
bool InputManager::CleanUp()
{
	bool ret = true;

	context_filter.clear();
	current_action.clear();
	listeners.clear();

	return ret;
}

void InputManager::InputDetected(int button, EVENTSTATE state)
{
	if (next_input_change == false)
	{
		std::map<GAMECONTEXT, std::multimap<int, INPUTEVENT>>::iterator cont = context_filter.find(actions_filter);

		if (cont != context_filter.end())
		{
			//contexts actions
			multimap<int, INPUTEVENT>::iterator actions = cont->second.find(button);
			
			if (actions != cont->second.end())
			{
				std::pair<INPUTEVENT, EVENTSTATE> new_current_action;
				new_current_action.first = (*actions).second;
				new_current_action.second = state;
				current_action.insert(new_current_action);
			}	
		}
	}
	else
	{
		ChangeEventButton(button);
	}
}

void InputManager::JoystickDetected(int axis, JSTATE state)
{
	if (actions_filter == IN_GAME)
	{
		std::pair<INPUTEVENT, EVENTSTATE> new_current_action;

		switch (axis)
		{
		case SDL_CONTROLLER_AXIS_LEFTX:
			if (state == J_POSITIVE)
			{
				new_current_action.first = MRIGHT;
				new_current_action.second = E_REPEAT;
				current_action.insert(new_current_action);
			}
			else
			{
				new_current_action.first = MLEFT;
				new_current_action.second = E_REPEAT;
				current_action.insert(new_current_action);
			}
			break;

		case SDL_CONTROLLER_AXIS_LEFTY:

			if (state == J_POSITIVE)
			{
				new_current_action.first = MDOWN;
				new_current_action.second = E_REPEAT;
				current_action.insert(new_current_action);
			}
			else
			{
				new_current_action.first = MUP;
				new_current_action.second = E_REPEAT;
				current_action.insert(new_current_action);
			}
			break;

		case SDL_CONTROLLER_AXIS_RIGHTX:
			if (state == J_POSITIVE)
			{
				new_current_action.first = LOOKRIGHT;
				new_current_action.second = E_REPEAT;
				current_action.insert(new_current_action);
			}
			else
			{
				new_current_action.first = LOOKLEFT;
				new_current_action.second = E_REPEAT;
				current_action.insert(new_current_action);
			}

			break;

		case SDL_CONTROLLER_AXIS_RIGHTY:
			if (state == J_POSITIVE)
			{
				new_current_action.first = LOOKDOWN;
				new_current_action.second = E_REPEAT;
				current_action.insert(new_current_action);
			}
			else
			{
				new_current_action.first = LOOKUP;
				new_current_action.second = E_REPEAT;
				current_action.insert(new_current_action);
			}

			break;

		}
	}
	
}

void InputManager::ChangeInputEvent(INPUTEVENT change_ev)
{
	next_input_change = true;
	event_to_change = change_ev;
}

bool InputManager::ChangeEventButton(int new_button)
{
	bool ret = false;
/*
	//Look if the new button is actually asigned
	multimap<int, INPUTEVENT>::iterator tmp = actions.find(new_button);

	if (tmp != actions.end())
	{
		LOG("This button is actually in another action");
		return ret;
	}

	//Look for the event to erase it
	tmp = actions.begin();
	while ((*tmp).second != event_to_change)
		tmp++;
	actions.erase(tmp);

	//This is the event with the new button
	std::pair<int, INPUTEVENT> event_changed;
	event_changed.first = new_button;
	event_changed.second = event_to_change;
	actions.insert(event_changed);

	//Reset the variables
	next_input_change = false;
	event_to_change = NO_EVENT;

	ret = true;

*/
	return ret;
}

EVENTSTATE InputManager::EventPressed(INPUTEVENT action) const
{
	multimap<INPUTEVENT, EVENTSTATE>::const_iterator tmp = current_action.find(action);

	if (tmp != current_action.end())
		return tmp->second;

	return E_NOTHING;
}

void InputManager::AddListener(InputListener* new_listener)
{
	//To improve this: Search if the listener is actually in the list

	if (new_listener)
	{
		new_listener->input_active = true;
		listeners.push_back(new_listener);
	}

}

void InputManager::CallListeners()
{
	if (!current_action.empty())
	{
		for (list<InputListener*>::iterator it = listeners.begin(); it != listeners.end(); it++)
		{
			if ((*it)->input_active)
			{
				std::multimap<INPUTEVENT, EVENTSTATE>::iterator frame_actions = current_action.begin();
				while (frame_actions != current_action.end())
				{
					(*it)->OnInputCallback(frame_actions->first, frame_actions->second);

					frame_actions++;
				}
			}
		}
	}
}

GAMECONTEXT InputManager::GetGameContext()
{
	return actions_filter;
}

void InputManager::SetGameContext(GAMECONTEXT context)
{
	actions_filter = context;
}


