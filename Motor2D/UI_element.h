#ifndef _UI_ELEMENT_H_
#define _UI_ELEMENT_H_

#include "p2Point.h"
#include "j1Input.h"
#include "j1Timer.h"
#include "SDL\include\SDL.h"
#include "SDL\include\SDL_rect.h"

enum UI_TYPE
{
	UNDEFINED,
	IMAGE,
	IMAGE_NOT_IN_ATLAS,
	STRING,
	BUTTON,
	TEXT_BOX,
	SCROLL,
	HEART,
	STAMINA_BAR,
	COUNTER
};

enum INTERACTIVE_STATE
{
	NOTHING,
	OVER_ELEMENT,
	CLICK_ELEMENT
};

enum SCROLL_TYPE
{
	NO_SCROLL = 0,
	FREE_SCROLL,
	Y_SCROLL,
	X_SCROLL
};

enum ANIMATION_TRANSITION
{
	NO_AT,

	//animations

	//Separator
	AT_SEPARATOR,

	//transitions
	T_FADE,
	T_FLY_UP,
	T_MOVE_UP,
	T_MOVE_DOWN

};

struct j1Module;

class UI_element
{
public:

	UI_TYPE element_type;
	SDL_Rect Interactive_box;

	INTERACTIVE_STATE state = INTERACTIVE_STATE::NOTHING;

	list<UI_element*> Childs;
	const UI_element* Parent = nullptr;

	uint layer = 1;
	int tab_order = 0;

	bool active;
	SCROLL_TYPE draggable;

	j1Module* my_module;
	bool into_render = false;

public:
	
	UI_element(UI_TYPE type, j1Module*);
	UI_element(UI_TYPE type, SDL_Rect detection_box, bool active = true, SCROLL_TYPE draggable = FREE_SCROLL);
	UI_element(const UI_element* other_element);
	~UI_element();

	//loop functions
	virtual bool Start() { return true; };
	virtual bool Update();
	virtual bool Update_Draw();
	virtual bool Handle_input() { return true; };		//Calls the UI callbacks 

	//Childs and parents functions
	UI_element* AddChild(UI_element* new_child);				
	
	//Adds the element to the tab
	void Add_to_Tab();
	
	//Set the elements atributes
	void Set_Interactive_Box(SDL_Rect);
	void Set_Active_state(bool);
	void Set_Drag_Type(SCROLL_TYPE);

	//Childs functions
	const UI_element* Set_Parent(const UI_element* parent);	//set the parent
	void Child_Update_Draw();								//Draw all childs
	void Child_Update();									//Update childs

															//Check state functions
	bool Mouse_is_in(const iPoint& mouse_pos);				//Check if Mouse is inside the interaction box


	UI_element* get_higher_child();							//Returns the child which is higher when you click inside

	
	void Check_state();										//Changes the state of the element
	void Return_state();									//Returns to state nothing when key up
	void Drag_element();									//Moves the element

	void GoToRender();
	void QuitFromRender();
	void QuitChildsFromRender();

	//animations and transitions
	void					LookAnimationTransition();
	ANIMATION_TRANSITION	GetCurrentTransition() const;
	void					SetAnimationTransition(ANIMATION_TRANSITION, int, iPoint);

private:

	void ChildsGoToRender();

	//animations and transitions
	ANIMATION_TRANSITION	current_animation = NO_AT;
	ANIMATION_TRANSITION	current_transition = NO_AT;

	bool					doing_animation = false;
	bool					doing_transition = false;
	
	//timers
	j1Timer					animation_timer;
	j1Timer					transition_timer;
	int						anim_duration = 0;
	int						trans_duration = 0;
	int						current_trans_time = 0;
	int						current_anim_time = 0;
	
	//bool					must_disable = false;

	//bezier destination
	iPoint					trans_origin = iPoint(0, 0);
	iPoint					trans_destiny = iPoint(0, 0);

	//transitions
	void					Fade();
	void					MoveUp();
	void					MoveDown();
	void					FlyUp();




};


#endif // !_UI_ELEMENT_H_
