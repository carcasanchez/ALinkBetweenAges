#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "Player.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "Module_entites_manager.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Gui.h"
#include "UI_element.h"
#include "UI_Image.h"
#include "UI_Text_Box.h"
#include "UI_scroll.h"
#include "UI_Button.h"
#include "UI_String.h"
#include "j1Scene.h"

j1Scene::j1Scene() : j1Module()
{
	name = ("scene");

}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	if (App->map->Load("iso_walk.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->Load("maps/path2.png");

	// TODO 3: Create the image (rect {485, 829, 328, 103}) and the text "Hello World" as UI elements
	//App->gui->CreateElement({ 0, 0 }, UI_TYPE::IMAGE_NOT_IN_ATLAS, { 0, 0, 1920, 1080 }, 0);


	/*
	//Exam
	
	//Exercise 1 -> Setting all the elements
	Background = (UI_Image*)App->gui->Add_element(UI_TYPE::IMAGE, this);
	Background->Set_Image_Texture({ 0, 1600, 1024, 576 });
	
	window = (UI_Image*)App->gui->Add_element(UI_TYPE::IMAGE, this);
	window->Set_Interactive_Box({ 50, 60, 455, 497 });
	window->Set_Image_Texture({ 15,512,455,497 });
	
	title = (UI_String*)App->gui->Add_element(UI_TYPE::STRING, this);
	title->Set_String("Select Your Character");
	title->Set_Interactive_Box({ 120, 50, 0, 0 });

	button_over = (UI_Image*)App->gui->Add_element(UI_TYPE::IMAGE, this); 
	button_over->Set_Image_Texture({ 2,112,228,67 });
	button_over->Set_Active_state(false);
	
	clicked_Button = (UI_Image*)App->gui->Add_element(UI_TYPE::IMAGE, this);
	clicked_Button->Set_Image_Texture({ 413,168,228,67 });
	clicked_Button->Set_Active_state(false);
	
	button_image = (UI_Image*)App->gui->Add_element(UI_TYPE::IMAGE, this);
	button_image->Set_Image_Texture({ 644,168,228,67 });
	button_image->Set_Active_state(false);


	Button = (UI_Button*)App->gui->Add_element(UI_TYPE::BUTTON, this);
	Button->Set_Drag_Type(NO_SCROLL); 
	Button->Set_Interactive_Box({ 120, 380, 228, 67 });
	Button->Set_Button_Image(button_image);

	Button_Label = (UI_String*)App->gui->Add_element(UI_TYPE::STRING, this);
	Button_Label->Set_String("Continue");
	Button_Label->Set_Interactive_Box({ 50, 20, 0, 0 });

	Big_heroe_image = (UI_Image*)App->gui->Add_element(UI_TYPE::IMAGE, this);
	Big_heroe_image->Set_Image_Texture({ 0, 1024, 1025, 574 });
	Big_heroe_image->Set_Interactive_Box({ 0,0,1025,574 });
	Big_heroe_image->Set_Drag_Type(NO_SCROLL);

	Slider_image = (UI_Image*)App->gui->Add_element(UI_TYPE::IMAGE, this);
	Slider_image->Set_Active_state(false);
	Slider_image->Set_Image_Texture({ 900, 314, 68, 54 });
	
	Heroe_images = (UI_Image*)App->gui->Add_element(UI_TYPE::IMAGE, this);
	Heroe_images->Set_Image_Texture({ 0, 290, 300, 169 });
	Heroe_images->Set_Active_state(false);

	Heroe_selector = (UI_Scroll*)App->gui->Add_element(UI_TYPE::SCROLL, this);
	Heroe_selector->Set_Interactive_Box({ 0, 0, 68, 54 });
	Heroe_selector->Set_Stop_Box({ 130, 200, 300, 169 });
	Heroe_selector->Set_Slider_image(Slider_image);
	Heroe_selector->Set_Stop_box_image(Heroe_images);
	Heroe_selector->Set_Camera({ 592, 151, 339, 271 });
	Heroe_selector->Add_Camera_element(Big_heroe_image);

	Exam_screen = App->gui->CreateScreen(&UI_element(UNDEFINED, { 0, 0, 1024, 576 }));

	Exam_screen->AddChild(Background);
	Exam_screen->AddChild(window);
	Exam_screen->AddChild(button_image);
	Exam_screen->AddChild(button_over);
	Exam_screen->AddChild(clicked_Button);

	window->AddChild(title);
	window->AddChild(Button);
	window->AddChild(Heroe_selector);

	Button->AddChild(Button_Label);

	//Exercise 2 Setting the scroll elements
	

	


	soldier0 = (Player*)App->entity_manager->create({ 15, 5 });
	soldier0->Set_texture({ 3, 2, 28, 36 });

	soldier1 = (Player*)App->entity_manager->create({ 24,0 });
	soldier1->Set_texture({ 3, 2, 28, 36 });

	soldier2 = (Player*)App->entity_manager->create({ 20,6 });
	soldier2->Set_texture({ 3, 2, 28, 36 });

	soldier3 = (Player*)App->entity_manager->create({ 24,10 });
	soldier3->Set_texture({ 3, 2, 28, 36 });
	*/
	

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	
	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if(origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}
	
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += SDL_ceil(100 * dt);

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= SDL_ceil(100 * dt);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += SDL_ceil(100 * dt);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= SDL_ceil(100 * dt);

	App->map->Draw();
	
	// Debug pathfinding ------------------------------
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x, p.y);


	const vector<iPoint>* path = App->pathfinding->GetLastPath();

	for(uint i = 0; i < path->size(); ++i)
	{
	iPoint pos = App->map->MapToWorld(path->at(i).x, path->at(i).y);
	App->render->Blit(debug_tex, pos.x, pos.y);
	}
	
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool j1Scene::On_GUI_Callback(UI_element* elem, GUI_INPUT gui_input)
{
	if (active)
	{
		switch (gui_input)
		{
		case MOUSE_IN:
			if (elem == Button)
				Button->Set_Button_Image(button_over);
			break;

		case MOUSE_OUT:
			if (elem == Button)
				Button->Set_Button_Image(button_image);
			break;

		case LEFT_MOUSE_DOWN:
			if (elem == Button)
				Button->Set_Button_Image(clicked_Button);
			break;

		case LEFT_MOUSE_REPEAT:
			if (elem == Button)
				Button->Set_Button_Image(clicked_Button);
			break;
		}

	}
	return true;
}
