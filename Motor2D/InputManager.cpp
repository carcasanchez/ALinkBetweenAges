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
bool InputManager::awake(pugi::xml_node& conf)
{
	bool ret = true;

	//Loading shortcuts path xml
	inputs_file_path = conf.child("shortcuts_path").attribute("value").as_string();

	return ret;
}

// Called before all Updates
bool InputManager::preUpdate()
{
	bool ret = true;

	//DOWN
	//Code with down queues here
	if (!App->input->down_queue.empty())
	{
		for (int i = 0; i < App->input->down_queue.size(); i++)
		{
			list<ShortCut*>::iterator it = shortcuts_list.begin();
			while (it != shortcuts_list.end())
			{
				if (App->input->down_queue.front() + i == (*it)->command && (*it)->type == DOWN)
					(*it)->active = true;
				++it;
			}
		}
	}
	//UP
	//Code with up queues here
	if (!App->input->up_queue.empty())
	{
		for (int i = 0; i < App->input->up_queue.size(); i++)
		{
			list<ShortCut*>::iterator it = shortcuts_list.begin();
			while (it != shortcuts_list.end())
			{
				if (App->input->up_queue.front() + i == (*it)->command && (*it)->type == UP)
					(*it)->active = false;
				++it;
			}
		}
	}
	//REPEAT
	//Code with repeat queues here
	if (!App->input->repeat_queue.empty())
	{
		for (int i = 0; i < App->input->repeat_queue.size(); i++)
		{
			list<ShortCut*>::iterator it = shortcuts_list.begin();
			while (it != shortcuts_list.end())
			{
				if (App->input->repeat_queue.front() + i == (*it)->command && (*it)->type == REPEAT)
					(*it)->active = true;
				++it;
			}
		}
	}

	return ret;
}

bool InputManager::update(float dt)
{
	bool ret = true;

	// Check for shortcut change
	list<ShortCut*>::iterator it = shortcuts_list.begin();
	while (it != shortcuts_list.end())
	{
		if ((*it)->ready_to_change)
		{
			static SDL_Event event;

			// COMPLETELY STOPS THE GAME...
			SDL_WaitEvent(&event);

			if (event.type == SDL_KEYDOWN)
			{
				std::string code = SDL_GetScancodeName(event.key.keysym.scancode);

				bool keyAlreadyAssigned = false;

				list<ShortCut*>::iterator it2 = shortcuts_list.begin();
				for (;it2 != shortcuts_list.end() && !keyAlreadyAssigned; it2++)
				{
					keyAlreadyAssigned = (strcmp((*it2)->command.c_str(), code.c_str()) == 0);
				}

				if (!keyAlreadyAssigned)
				{
					(*it)->command = code;
					ChangeShortcutCommand((*it));

					/*list<ShortCut*>::iterator it = shortcuts_list.begin();
					while (it != shortcuts_list.end())
					{
						(*it)->active = false;
						++it;
					}*/
				}
				else
				{
					LOG("Key already assigned");
				}
			}

			(*it)->ready_to_change = false;

			return ret;
		}

		++it;
	}

	return ret;
}

// Called after all Updates
bool InputManager::postUpdate()
{
	bool ret = true;

	list<ShortCut*>::iterator it = shortcuts_list.begin();

	while (it != shortcuts_list.end())
	{
		(*it)->active = false;
		++it;
	}

	return ret;
}

// Called before quitting
bool InputManager::cleanUp()
{
	bool ret = true;

	shortcuts_list.clear();

	return ret;
}

bool InputManager::CheckShortcut(ShortCutID id)
{
	bool ret = false;

	int i = 0;
	std::list<ShortCut*>::iterator it = shortcuts_list.begin();

	while (it != shortcuts_list.end())
	{
		if (i == int(id))
		{
			ret = (*it)->active;
			break;
		}

		i++;
		it++;
	}

	return ret;
}

void InputManager::ChangeShortcutCommand(ShortCut* shortcut)
{
	// manage input manager panel
	/*shortcut->command_label->SetText(shortcut->command, app->font->smallFont);

	iPoint posLabel = shortcut->command_label->getLocalPosition();
	iPoint posDecor = shortcut->decor->getLocalPosition();
	posLabel.x = posDecor.x + 63 - ((int(shortcut->command.size()) - 1) * 5);
	shortcut->command_label->SetLocalPosition(posLabel);*/

	shortcut->ready_to_change = false;
}