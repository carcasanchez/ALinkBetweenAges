#include "UI_element.h"
#include "UI_String.h"
#include "UI_Text_Box.h"
#include "UI_Button.h"
#include "j1Gui.h"
#include "UI_scroll.h"
#include "j1App.h"

UI_element::UI_element(UI_TYPE type, j1Module* callback_module) : element_type(type), my_module(callback_module), Interactive_box({ 0,0,0,0 }), active(true), draggable(FREE_SCROLL) {}

UI_element::UI_element(UI_TYPE type, SDL_Rect detection_box, bool act, SCROLL_TYPE drag) : element_type(type), Interactive_box(detection_box), active(act), draggable(drag)
{

}

UI_element::UI_element(const UI_element* other) : element_type(other->element_type), Interactive_box(other->Interactive_box), active(other->active), draggable(other->draggable){}

UI_element* UI_element::AddChild(UI_element* new_child)
{
	UI_element* ret = new_child;

	ret->Set_Parent(this);

	if (ret != nullptr)
		Childs.push_back(ret);

	return ret;
}

const UI_element* UI_element::Set_Parent(const UI_element* new_Parent)
{
	Parent = new_Parent;
	Interactive_box.x += Parent->Interactive_box.x;
	Interactive_box.y += Parent->Interactive_box.y;
	layer += Parent->layer;

	return Parent;
}

bool UI_element::Update()
{
	Child_Update();
	Handle_input();

	return true;
}

bool UI_element::Update_Draw()
{
	Child_Update_Draw();

	return true;
}

bool UI_element::Mouse_is_in(const iPoint& mouse_pos)
{
	return ((mouse_pos.x > Interactive_box.x) && (mouse_pos.x <= (Interactive_box.x + Interactive_box.w)) && (mouse_pos.y > Interactive_box.y) && (mouse_pos.y <= (Interactive_box.y + Interactive_box.h)));
}

void UI_element::Child_Update_Draw()
{
	for (list<UI_element*>::iterator item_child = Childs.begin(); item_child != Childs.cend(); item_child++)
		(*item_child)->Update_Draw();
}

void UI_element::Child_Update()
{
	for (list<UI_element*>::iterator item_child = Childs.begin(); item_child != Childs.cend(); item_child++)
		(*item_child)->Update();
	
}

void UI_element::Check_state()
{
	int x, y;
	App->input->GetMousePosition(x, y);

	//Check if the element selected is this
	if (App->gui->element_selected == nullptr)
	{
		//Check if mouse is inside the interaction box [inside = true]
		if (Mouse_is_in({ x, y }))
		{
			//Check if you Click inside the element
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			{

				//Check If you click a child which is inside the element box
				if (get_higher_child() != nullptr)
					state = OVER_ELEMENT;
				
				else
				{
					//Here the element will be the selected and will change his state
					App->gui->element_selected = this;
					App->gui->focus_element = this;
					state = CLICK_ELEMENT;
				}

			}
			//If you don't click changes his state to over element
			else state = OVER_ELEMENT;
		}
		//If mouse is not In his returns to normal state
		else state = NOTHING;
	}
	else 
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && App->gui->element_selected->element_type != TEXT_BOX)
			App->gui->element_selected = nullptr;
	}
	

}

void UI_element::Return_state()
{
	if (App->gui->element_selected)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && App->gui->element_selected->element_type != TEXT_BOX)
		{
			App->gui->element_selected = nullptr;
			App->gui->focus_element = nullptr;
			state = NOTHING;
		}
	}
}

void UI_element::Drag_element()
{

	int x = 0, y = 0;
	App->input->GetMouseMotion(x, y);

	SCROLL_TYPE temp = draggable;

	if (Parent)	
		draggable = Parent->draggable;
	
	//Looks the argument draggable and moves the element 
	switch (draggable)
	{
	case FREE_SCROLL:
		Interactive_box.x += x;
		Interactive_box.y += y;
		break;

	case Y_SCROLL:
		Interactive_box.y += y;
		break;

	case X_SCROLL:
		Interactive_box.x += x;
		break;
	}
	
	//Moves the childs
	for (list<UI_element*>::iterator item_child = Childs.begin(); item_child != Childs.cend(); item_child++)
		(*item_child)->Drag_element();

	draggable = temp;

}

void UI_element::Add_to_Tab()
{
	tab_order = App->gui->Get_tabs() + 1;
	App->gui->Actualize_tabs();

	if (App->gui->element_selected == nullptr)
		App->gui->focus_element = this;
	
}


UI_element* UI_element::get_higher_child()
{
	UI_element* temp = nullptr;

	if (Childs.size() > 0)
	{
		for (list<UI_element*>::iterator item_child = Childs.begin(); item_child != Childs.cend(); item_child++)
		{
			if ((*item_child)->state == OVER_ELEMENT && (*item_child)->layer >= this->layer)
			{
				temp = (*item_child);
				temp->state = CLICK_ELEMENT;
				App->gui->element_selected = temp;
				App->gui->focus_element = temp;
			}
		}
	}

	return temp;
}

void UI_element::Set_Interactive_Box(SDL_Rect new_rect)
{
	Interactive_box = new_rect;
}

void UI_element::Set_Active_state(bool act)
{
	active = act;
}

void UI_element::Set_Drag_Type(SCROLL_TYPE type)
{
	draggable = type;
}


