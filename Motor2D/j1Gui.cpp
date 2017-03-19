#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Window.h"
#include "UI_Text_Box.h"
#include "UI_scroll.h"
#include "UI_Image.h"
#include "UI_Interactive_String.h"
#include "UI_String.h"
#include "UI_Button.h"

j1Gui::j1Gui() : j1Module()
{
	name = ("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");
	scale_factor = scale_factor / App->win->GetScale();

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.c_str());
	//Other_images.add(App->tex->Load("gui/login_background.png"));


	return true;

}

// Update all guis
bool j1Gui::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
		Go_Next_Tab();

	for (list<UI_element*>::iterator item_screen = Screen_elements.begin(); item_screen != Screen_elements.end(); item_screen++)
		(*item_screen)->Update();
	
	return true;
}

bool j1Gui::Update()
{
	
	

	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	for (list<UI_element*>::iterator item_screen = Screen_elements.begin(); item_screen != Screen_elements.end(); item_screen++)
		(*item_screen)->Update_Draw();

	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	for (list<UI_element*>::iterator item_screen = Screen_elements.begin(); item_screen != Screen_elements.end(); item_screen++)
		Screen_elements.erase(item_screen);

	Screen_elements.clear();


	return true;
}

UI_element * j1Gui::CreateScreen(UI_element * new_element)
{
	UI_element* ret = nullptr;

	if (new_element)
	{
		ret = new_element;
		Screen_elements.push_back(new_element);
	}
	else
	{
		ret = Add_element(UNDEFINED, this);
		if (ret)
			Screen_elements.push_back(ret);
	}

	return ret;
}

UI_element* j1Gui::Add_element(UI_TYPE TYPE, j1Module* element_module)
{
	UI_element* ret = nullptr;

	switch (TYPE)
	{
	case UNDEFINED:
		ret = new UI_element(TYPE, element_module);
		break;
	case IMAGE:
		ret = new UI_Image(TYPE, element_module);
		break;
	case STRING:
		ret = new UI_String(TYPE, element_module);
		break;
	case BUTTON:
		ret = new UI_Button(TYPE, element_module);
		break;
	case TEXT_BOX:
		ret = new UI_Text_Box(TYPE, element_module);
		break;
	case SCROLL:
		ret = new UI_Scroll(TYPE, element_module);
		break;
	
	}

	return ret;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

const SDL_Texture* j1Gui::Get_Other_Textures(uint id) const
{
	if (id < Other_images.size())
	{
		list<SDL_Texture*>::const_iterator temp = Other_images.begin();
		for (int i = 0; i < id; i++)
			temp++;

		return *temp;

	}

	return nullptr;	
}


int j1Gui::Get_tabs() const
{
	return num_of_tabs;
}

void j1Gui::Actualize_tabs()
{
	num_of_tabs++;
}

void j1Gui::Go_Next_Tab()
{
	if (focus_element != nullptr)
	{
		list<UI_element*>::iterator item = Screen_elements.begin();
		while (item != Screen_elements.end())
		{
			Look_for(*item);
			item++;
		}
	}

}

void j1Gui::Look_for(const UI_element* looked_element)
{
	list<UI_element*>::iterator childs_looker = Screen_elements.begin();
	for (; childs_looker != Screen_elements.end(); childs_looker++)
	{
		if (focus_element->tab_order < num_of_tabs)		//Look if the Focus element is the last one
		{
			if ((*childs_looker)->tab_order == focus_element->tab_order + 1)	//Look for the next element in the tab order
			{
				element_selected = (*childs_looker);
				focus_element = (*childs_looker);
				(*childs_looker)->state = INTERACTIVE_STATE::OVER_ELEMENT;
				break;
			}

		}
		else
		{
			if ((*childs_looker)->tab_order == 1)
			{
				element_selected = (*childs_looker);
				focus_element = (*childs_looker);
				(*childs_looker)->state = INTERACTIVE_STATE::OVER_ELEMENT;
				break;
			}
		}
	}



	if (childs_looker == Screen_elements.end())
	{
		for (list<UI_element*>::const_iterator item = looked_element->Childs.begin(); item != looked_element->Childs.cend(); item++)
			Look_for(*item);
	}


}

// class Gui ---------------------------------------------------


