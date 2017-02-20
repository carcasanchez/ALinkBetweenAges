#include "UI_scroll.h"
#include "j1Gui.h"
#include "j1App.h"
#include "j1Render.h"

UI_Scroll::UI_Scroll(UI_TYPE type, j1Module* callback) : UI_element(type, callback), Camera({ 0,0,0,0 }), Camera_inner_box({ 0,0,0,0 }) {}

UI_Scroll::UI_Scroll(UI_TYPE type, SDL_Rect detection_box, const UI_Image* slider, const UI_Image* slide_box, bool act, SCROLL_TYPE drag) : UI_element(type, detection_box, act, drag), Slider((UI_Image*)slider), Slide_box((UI_Image*)slide_box)
{
	Interactive_box.x = slide_box->Interactive_box.x;
	Interactive_box.y = slide_box->Interactive_box.y;
}

UI_Scroll::UI_Scroll(const UI_Scroll* other) : UI_element(other->element_type, other->Interactive_box, other->active, other->draggable), Slider(other->Slider), Slide_box(other->Slide_box)
{
	Interactive_box.x = other->Slide_box->Interactive_box.x;
	Interactive_box.y = other->Slide_box->Interactive_box.y;
}

UI_Scroll::~UI_Scroll()
{
	for (list<UI_element*>::iterator camera_item = Camera_elements.begin(); camera_item != Camera_elements.cend(); camera_item++)
		Camera_elements.erase(camera_item);
}

bool UI_Scroll::Update()
{
	Handle_input();

	if (App->gui->element_selected == this && draggable)
		Drag_element();

	if (App->gui->element_selected == Parent)
		Move_stop_box();

	if (Mouse_inside_Camera_box())
		Scroll_Wheel();

	Stop();

	Move_elements();
	
	return true;
}

bool UI_Scroll::Update_Draw()
{
	if (active)
	{	
		App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), Stop_box.x - App->render->camera.x, Stop_box.y - App->render->camera.y, &Slide_box->Image);
		App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), Interactive_box.x - App->render->camera.x, Interactive_box.y - App->render->camera.y, &Slider->Image);
		

		Child_Update_Draw();
	}

	
	//Exercise 3 The scroll Camera view
	SDL_RenderSetViewport(App->render->renderer, &Camera);

	for (list<UI_element*>::iterator camera_item = Camera_elements.begin(); camera_item != Camera_elements.cend(); camera_item++)
		(*camera_item)->Update_Draw();
	
	SDL_RenderSetViewport(App->render->renderer, nullptr);
	

	return true;
}

bool UI_Scroll::Handle_input()
{
	int x, y;
	App->input->GetMousePosition(x, y);

	if (this->Mouse_is_in({ x, y }) || this->Mouse_inside_Camera_box())
	{
		my_module->On_GUI_Callback(this, MOUSE_IN);
		state = OVER_ELEMENT;
	}
	else
	{
		my_module->On_GUI_Callback(this, MOUSE_OUT);
		state = NOTHING;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && App->gui->element_selected == nullptr)
	{
		if (this->Mouse_is_in({ x, y }))
		{
			if (get_higher_child() != nullptr)
			{
				state = OVER_ELEMENT;
			}
			else
			{
				my_module->On_GUI_Callback(this, LEFT_MOUSE_DOWN);
				App->gui->element_selected = this;
				App->gui->focus_element = this;
				state = CLICK_ELEMENT;
			}
		}
		else
		{
			App->gui->element_selected = nullptr;
			App->gui->focus_element = nullptr;
			state = NOTHING;
		}
	}

	if (App->gui->element_selected == nullptr) return true;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		my_module->On_GUI_Callback(this, LEFT_MOUSE_REPEAT);
		state = CLICK_ELEMENT;
	}
	return true;
}

void UI_Scroll::Set_Stop_Box(SDL_Rect new_box)
{
	Stop_box = new_box;

	if (Parent)
	{
		Interactive_box.x = Stop_box.x;
		Interactive_box.y = Stop_box.y;
	}
	
}

void UI_Scroll::Set_Camera(SDL_Rect new_camera)
{
	Camera = new_camera;
	Camera_inner_box = new_camera;
}

void UI_Scroll::Stop()
{
	if (Interactive_box.y < Stop_box.y)
		Interactive_box.y = Stop_box.y;
	
	if ((Interactive_box.y + Interactive_box.h) > (Stop_box.y + Stop_box.h))
		Interactive_box.y = (Stop_box.y + Stop_box.h) - Interactive_box.h;

	if ( Interactive_box.x < Stop_box.x)
		Interactive_box.x = Stop_box.x;

	if((Interactive_box.x + Interactive_box.w) > (Stop_box.x + Stop_box.w))
		Interactive_box.x = (Stop_box.x + Stop_box.w) - Interactive_box.w;


}

void UI_Scroll::Move_elements()
{
	
	int percent_y = ((Interactive_box.y - Stop_box.y) * 100) / (Stop_box.h - Interactive_box.h);
	int percent_x = ((Interactive_box.x - Stop_box.x) * 100) / (Stop_box.w - Interactive_box.w);

	if (state == CLICK_ELEMENT || state == OVER_ELEMENT)
	{
		list<UI_element*>::iterator camera_item = Camera_elements.begin();
		list<iPoint>::iterator pos = Camera_element_position.begin();

		for (; camera_item != Camera_elements.cend(); camera_item++, pos++)
		{
			(*camera_item)->Interactive_box.x = -(((Camera_inner_box.w - Camera.w) * percent_x) / 100) + (*pos).x;
			(*camera_item)->Interactive_box.y = -(((Camera_inner_box.h - Camera.h) * percent_y) / 100) + (*pos).y;
		}
	}
}

void UI_Scroll::Move_stop_box()
{
	int x = 0, y = 0;
	App->input->GetMouseMotion(x, y);

	Stop_box.x += x;
	Stop_box.y += y;

}

void UI_Scroll::Scroll_Wheel()
{
	
	int y;
	App->input->GetMouseWheel(y);

	switch (draggable)
	{
	case Y_SCROLL || FREE_SCROLL:
		Interactive_box.y -= y;
		break;

	case X_SCROLL:
		Interactive_box.x -= y;
		break;
	}
	

}

bool UI_Scroll::Mouse_inside_Camera_box()
{
	int x, y;
	App->input->GetMousePosition(x, y);
	
	if (((x > Stop_box.x) && (x <= (Stop_box.x + Stop_box.w)) && (y > Stop_box.y) && (y <= (Stop_box.y + Stop_box.h))))
		return true;

	if (((x > Camera.x) && (x <= (Camera.x + Camera.w)) && (y > Camera.y) && (y <= (Camera.y + Camera.h))))
		return true;

	return false;
}

void UI_Scroll::Add_Camera_element(UI_element* new_item)
{
	if (new_item)
	{
		Camera_elements.push_back(new_item);

		switch (draggable)
		{
		case Y_SCROLL:
			if ((new_item->Interactive_box.y + new_item->Interactive_box.h) > Camera_inner_box.h)
				Camera_inner_box.h = new_item->Interactive_box.y + new_item->Interactive_box.h;
			break;

		case X_SCROLL:
			if ((new_item->Interactive_box.x + new_item->Interactive_box.w) > Camera_inner_box.w)
				Camera_inner_box.w = new_item->Interactive_box.x + new_item->Interactive_box.w;
			
			break;

		case FREE_SCROLL:
			if ((new_item->Interactive_box.y + new_item->Interactive_box.h) > Camera_inner_box.h)
				Camera_inner_box.h = new_item->Interactive_box.y + new_item->Interactive_box.h;
			
			if ((new_item->Interactive_box.x + new_item->Interactive_box.w) > Camera_inner_box.w)
				Camera_inner_box.w = new_item->Interactive_box.x + new_item->Interactive_box.w;
			
			break;
		}
	
		iPoint pos = { new_item->Interactive_box.x, new_item->Interactive_box.y };
		Camera_element_position.push_back(pos);

	}

	

}

SDL_Rect UI_Scroll::Get_Stop_Box() const
{
	return Stop_box;
}

void UI_Scroll::Set_Slider_image(UI_Image* Scroll_img)
{
	Slider = Scroll_img;
}

void UI_Scroll::Set_Stop_box_image(UI_Image* Box_img)
{
	Slide_box = Box_img;
}

