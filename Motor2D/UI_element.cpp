#include "UI_element.h"
#include "UI_String.h"
#include "UI_Text_Box.h"
#include "p2Log.h"
#include "UI_Button.h"
#include "j1Gui.h"
#include "j1Render.h"
#include "Bezier.h"
#include "UI_scroll.h"
#include "j1App.h"

UI_element::UI_element(UI_TYPE type, j1Module* callback_module) : element_type(type), my_module(callback_module), Interactive_box({ 0,0,0,0 }), active(true), draggable(FREE_SCROLL) {}

UI_element::UI_element(UI_TYPE type, SDL_Rect detection_box, bool act, SCROLL_TYPE drag) : element_type(type), Interactive_box(detection_box), active(act), draggable(drag)
{
	Childs.clear();
}

UI_element::UI_element(const UI_element* other) : element_type(other->element_type), Interactive_box(other->Interactive_box), active(other->active), draggable(other->draggable){}

UI_element::~UI_element()
{
	for (std::list<UI_element*>::iterator child = Childs.begin(); child != Childs.end(); child++)
		RELEASE(*child);

	if (into_render)
		App->render->EraseUiElement(this);

	Childs.clear();
}

UI_element* UI_element::AddChild(UI_element* new_child)
{
	UI_element* ret = new_child;

	ret->Set_Parent(this);

	if (ret != nullptr)
		Childs.push_back(ret);

	return ret;
}

bool UI_element::QuitChild(UI_element* del_child)
{
	for (std::list<UI_element*>::iterator it = Childs.begin(); it != Childs.end(); it++)
	{
		if ((*it) == del_child)
		{
			Childs.erase(it);
			RELEASE((del_child));			
			return true;
		}
	}

	return false;
}

const UI_element* UI_element::Set_Parent(const UI_element* new_Parent)
{
	Parent = new_Parent;
	/*
	Interactive_box.x += Parent->Interactive_box.x;
	Interactive_box.y += Parent->Interactive_box.y;
	layer += Parent->layer;
	*/

	return Parent;
}

bool UI_element::Update()
{
	Child_Update();

	LookAnimationTransition();

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
	if (Childs.size())
	{
		for (list<UI_element*>::iterator item_child = Childs.begin(); item_child != Childs.cend(); item_child++)
			(*item_child)->Update_Draw();
	}
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

void UI_element::GoToRender()
{
	if (into_render == false)
	{
		App->render->ui_elements.push_back(this);
		into_render = true;
		ChildsGoToRender();
	}
	
}

void UI_element::QuitFromRender()
{
	if (into_render)
	{
		for (std::list<UI_element*>::iterator it = App->render->ui_elements.begin(); it != App->render->ui_elements.end(); it++)
		{
			if ((*it) == this)
			{
				App->render->ui_elements.erase(it);
				into_render = false;
				break;
			}
		}
	}

	QuitChildsFromRender();
}

void UI_element::QuitChildsFromRender()
{
	for (list<UI_element*>::iterator item_child = Childs.begin(); item_child != Childs.cend(); item_child++)
		(*item_child)->QuitFromRender();
}

void UI_element::ChildsGoToRender()
{
	for (list<UI_element*>::iterator item_child = Childs.begin(); item_child != Childs.cend(); item_child++)
		(*item_child)->GoToRender();

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

void UI_element::LookAnimationTransition()
{
	if (current_animation != NO_AT)
	{

	}

	if (current_transition != NO_AT)
	{
		switch (current_transition)
		{
		case T_FADE_TO:
			FadeTo();
			break;

		case T_FADE_FROM:
			FadeFrom();
			break;

		case T_FLY_UP:
			FlyUp();
			break;

		case T_MOVE_UP:
			MoveUp();
			break;

		case T_MOVE_DOWN:
			MoveDown();
			break;

		case T_MOVE_LEFT:
			MoveLeft();
			break;

		case T_MOVE_RIGHT:
			MoveRight();
			break;
		}
	}
}

ANIMATION_TRANSITION UI_element::GetCurrentTransition() const
{
	return current_transition;
}

void UI_element::SetAnimationTransition(ANIMATION_TRANSITION new_anim_trans, int time, iPoint  trans_destination = { 0,0 })
{
	if (new_anim_trans < AT_SEPARATOR)
	{
		current_animation = new_anim_trans;
		anim_duration = time;
		doing_animation = false;
	}
	else
	{
		current_transition = new_anim_trans;
		trans_duration = time;
		trans_destiny = trans_destination;
		doing_transition = false;
	}
}

void UI_element::FadeTo()
{
	if (!doing_transition)
	{
		transition_timer.Start();
		current_trans_time = 0;
		doing_transition = true;
		alpha = 0;
	}
	current_trans_time = transition_timer.Read();

	if (current_trans_time < trans_duration)
	{
		float change_alpha = App->gui->bezier_curve->GetActualX(trans_duration, current_trans_time, CB_SLOW_MIDDLE);
		change_alpha = CLAMP01(change_alpha);
		alpha = 255 * change_alpha;
	}
	else
	{
		current_trans_time = 0;
		current_transition = NO_AT;
	
		return;
	}
}

void UI_element::FadeFrom()
{
	if (!doing_transition)
	{
		transition_timer.Start();
		current_trans_time = 0;
		doing_transition = true;
		alpha = 0;
	}
	current_trans_time = transition_timer.Read();

	if (current_trans_time < trans_duration)
	{
		float change_alpha = App->gui->bezier_curve->GetActualX(trans_duration, current_trans_time, CB_SLOW_MIDDLE);
		change_alpha = CLAMP01(change_alpha);
		alpha = 255 * (1 - change_alpha);
	}
	else
	{
		current_trans_time = 0;
		current_transition = NO_AT;

		return;
	}
}

void UI_element::MoveUp()
{
	if (!doing_transition)
	{
		transition_timer.Start();
		current_trans_time = 0;
		trans_origin = { Interactive_box.x, Interactive_box.y };
		doing_transition = true;
	}

	current_trans_time = transition_timer.Read();

	if (current_trans_time <= trans_duration)
	{

		Interactive_box.y = trans_origin.y + App->gui->bezier_curve->GetActualPoint(trans_origin, trans_destiny, trans_duration, current_trans_time, cbezier_type::CB_EASE_INOUT_BACK);

	}
	else
	{
		current_trans_time = 0;
		current_transition = NO_AT;
	}
}

void UI_element::MoveDown()
{

	if (!doing_transition)
	{
		transition_timer.Start();
		current_trans_time = 0;
		trans_origin = { Interactive_box.x, Interactive_box.y };
		doing_transition = true;
	}

	current_trans_time = transition_timer.Read();

	if (current_trans_time <= trans_duration)
	{
	
		Interactive_box.y = trans_origin.y - App->gui->bezier_curve->GetActualPoint(trans_origin, trans_destiny, trans_duration, current_trans_time, cbezier_type::CB_EASE_INOUT_BACK);
		
	}
	else
	{
		current_trans_time = 0;
		current_transition = NO_AT;
	}

}

void UI_element::MoveLeft()
{
	if (!doing_transition)
	{
		transition_timer.Start();
		current_trans_time = 0;
		trans_origin = { Interactive_box.x, Interactive_box.y };
		doing_transition = true;
	}

	current_trans_time = transition_timer.Read();

	if (current_trans_time <= trans_duration)
	{

		Interactive_box.x = trans_origin.x + App->gui->bezier_curve->GetActualPoint(trans_origin, trans_destiny, trans_duration, current_trans_time, cbezier_type::CB_EASE_INOUT_BACK);

	}
	else
	{
		current_trans_time = 0;
		current_transition = NO_AT;
	}
}

void UI_element::MoveRight()
{
	if (!doing_transition)
	{
		transition_timer.Start();
		current_trans_time = 0;
		trans_origin = { Interactive_box.x, Interactive_box.y };
		doing_transition = true;
	}

	current_trans_time = transition_timer.Read();

	if (current_trans_time <= trans_duration)
	{

		Interactive_box.x = trans_origin.x - App->gui->bezier_curve->GetActualPoint(trans_origin, trans_destiny, trans_duration, current_trans_time, cbezier_type::CB_EASE_INOUT_BACK);

	}
	else
	{
		current_trans_time = 0;
		current_transition = NO_AT;
	}
}

void UI_element::FlyUp()
{
	if (!doing_transition)
	{
		transition_timer.Start();
		current_trans_time = 0;
		trans_origin = { Interactive_box.x, Interactive_box.y };
		doing_transition = true;
	}

	current_trans_time = transition_timer.Read();

	if (current_trans_time <= trans_duration)
	{

		Interactive_box.y = trans_origin.y + App->gui->bezier_curve->GetActualPoint(trans_origin, trans_destiny, trans_duration, current_trans_time, cbezier_type::CB_FLY);

	}
	else
	{
		current_trans_time = 0;
		current_transition = NO_AT;
	}
}
