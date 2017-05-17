#include "j1CutSceneManager.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "Entity.h"
#include "Scene.h"
#include "j1Gui.h"
#include "UI_String.h"
#include "UI_element.h"
#include "j1Map.h"
#include "j1GameLayer.h"
#include "HUD.h"
#include "j1SceneManager.h"
#include "j1EntityManager.h"
#include "Player.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "p2Log.h"

//TODO 5.1: open the IntroCutscene.xml (in data.zip, in cutscenes folder) and fill THESE ELEMENTS correctly with this info:
//Image -> n = 0; name = BackgroundPokemon; x = -100; y = 0; tex_x = 0; tex_y = 0; tex_w = 1080; tex_h = 514; active = false; file = textures/PokemonBackground.png;
//Music -> n = 7; name = MainMusic; path = audio/music/ZELDA/ZeldaMainTitle.ogg;
//Fx -> n = 8; name = GoodSound; path = audio/fx/Rupee.wav; loops = 0;

//TODO 6.1: open the IntroCutscene.xml and fill THESE STEPS correctly with this info:
//Step 0 -> n = 0; start = 0; duration = -1; 
//			element -> name = BackgroundZelda action = move; dir = right;
//				movement -> dest_x = -100; dest_y = 0; speed = 3;
//Step 9 -> n = 9; start = 9; duration = 0; 
//			element -> name = GoodSound action = play; 

// CUTSCEME MANAGER -----------------------------------
j1CutSceneManager::j1CutSceneManager()
{
	name = "cutscenemanager";
}

j1CutSceneManager::~j1CutSceneManager()
{
}

bool j1CutSceneManager::Awake(pugi::xml_node& config)
{
	bool ret = false;
	if (config != NULL)
	{
		ret = true;
		//TODO 2: fill the paths list with the names of the config.xml file. 
		for (pugi::xml_node cutscene = config.child("cutscene"); cutscene != NULL; cutscene = cutscene.next_sibling())
			paths.push_back(cutscene.attribute("file").as_string());
	}

	

	return ret;
}

bool j1CutSceneManager::Start()
{
	App->inputM->AddListener(this);
	return true;
}

bool j1CutSceneManager::LoadCutscene(uint id)
{
	if (id > paths.size())
		return false;
	
	pugi::xml_document	cutscene_file;
	pugi::xml_node		cutscene_node;
	pugi::xml_node		elements_node;
	pugi::xml_node		steps_node;
	pugi::xml_node		temp;

	std::list<std::string>::iterator file = paths.begin();
	for (int i = 0; file != paths.end(); file++)
	{

		//TODO 4: stop iterating when the correct path is reached (take care of the passed id)
		if (i == id) //Check the load order of the paths to set the correct cutscene
		{
			//Load XML cutscene file
			cutscene_node = LoadXML(cutscene_file, (*file));

			//Create temp pointer
			Cutscene* temp_cutscene = new Cutscene();

			//Screen for ui cutscene elements
			temp_cutscene->cutscene_screen = App->gui->CreateScreen(temp_cutscene->cutscene_screen);

			temp_cutscene->id = cutscene_node.attribute("id").as_uint(0);			//Sets its identifier.
			temp_cutscene->name = cutscene_node.attribute("name").as_string("");	//Sets its name.

																					//LOAD ELEMENTS INVOLVED IN THE CUTSCENE --------------------------------------------------------------
			elements_node = cutscene_node.child("elements");

			//Load Scenes
			for (temp = elements_node.child("MAP").child("scene"); temp != NULL; temp = temp.next_sibling("scene"))
				temp_cutscene->LoadSceneName(temp);

			//Load NPCs
			for (temp = elements_node.child("NPC").child("npc"); temp != NULL; temp = temp.next_sibling("npc"))
				temp_cutscene->LoadNPC(temp);
			
			//Load Enemies
			for (temp = elements_node.child("ENEMIES").child("enemy"); temp != NULL; temp = temp.next_sibling("enemy"))
				temp_cutscene->LoadEnemy(temp);

			//Load Texts
			for (temp = elements_node.child("TEXTS").child("string"); temp != NULL; temp = temp.next_sibling("text"))
				temp_cutscene->LoadText(temp);
			

			//Load Images
			for (temp = elements_node.child("IMAGES").child("image"); temp != NULL; temp = temp.next_sibling("image"))
				temp_cutscene->LoadImg(temp);
			
			//Load Camera
			for (temp = elements_node.child("CAMERA").child("cam"); temp != NULL; temp = temp.next_sibling("cam"))
				temp_cutscene->LoadCam(temp);

			//Load Music
			for (temp = elements_node.child("MUSIC").child("mus"); temp != NULL; temp = temp.next_sibling("mus"))
				temp_cutscene->LoadMusic(temp);


			//Load Sound Fx

			// -----------------------------------------------------------------------------------

			// LOAD STEPS ---------------------------------
			steps_node = cutscene_node.child("steps").child("step");
			//TODO 6.2: Access the first step node and iterate it in order to load all of them by calling the correct function.
			while (steps_node != NULL)
			{
				temp_cutscene->LoadStep(steps_node, temp_cutscene);
				steps_node = steps_node.next_sibling();
			}


			// -----------------------------------------

			//Set the active_scene pointer to the current scene -------------------------
			active_cutscene = temp_cutscene;

			LOG("Cutscene '%s' loaded with %i elements and %i steps.", temp_cutscene->name.c_str(), temp_cutscene->GetNumElements(), temp_cutscene->GetNumSteps());
			break;
		}
		i++;
	}

	return true;
}

bool j1CutSceneManager::Update(float dt)
{
	bool ret = true;

	if (active_cutscene != nullptr)
	{
		if (active_cutscene->isFinished() == false) //Update the active cutscene until it's finished
		{
			active_cutscene->Update(dt);
		}
		else
		{
			FinishCutscene();
		}
	}

	return ret;
}

//Set to active the correct cutscene
bool j1CutSceneManager::StartCutscene(uint id)
{
	LoadCutscene(id); //Acces to the correct XML and load elements and steps of the cutscene

	if (active_cutscene != nullptr)
	{
		//TODO 9.2: Uncomment this and enjoy :)
		//Start the triggered cutscene
		active_cutscene->Start();

		//Change the game state (the Cutscene Manager now takes the control of the game)
		//App->scene->ChangeState(CUTSCENE);

		App->inputM->SetGameContext(GAMECONTEXT::IN_CUTSCENE);
		if (App->game->em->player)
		{
			App->game->em->player->playerState = CUTSCENE;
			App->game->em->player->actionState = IDLE;
		}

		LOG("%s cutscene activated", active_cutscene->name.c_str());
	}

	else
	{
		LOG("Cutscene not activated.");
	}

	return true;
}

//Set active_cutscene pointer to nullptr
bool j1CutSceneManager::FinishCutscene()
{
	bool ret = false;

	if (active_cutscene != nullptr)
	{
		if (active_cutscene->isFinished() == true)
		{
			LOG("%s cutscene deactivated", active_cutscene->name.c_str());

			//TODO 9.1: Load the destination map of the cutscene (if it has stored a map_id when accessed to the XML file (map_id > -1)).
			//Do this by calling the appropiate function of the intro scene.

			if (App->game->em->player && active_cutscene->id == 2)
			{
				App->SaveGame("saves.xml");
				
			}

			//TODO 10: Clear the cutscene and set active_cutsene pointer to nullptr.
			RELEASE(active_cutscene);

			//Return to INGAME state
			App->inputM->SetGameContext(GAMECONTEXT::IN_GAME);
			
			if(App->game->em->player != nullptr)
				App->game->em->player->playerState = ACTIVE;
		

			ret = true;
		}
	}
	return ret;
}

void j1CutSceneManager::OnInputCallback(INPUTEVENT action, EVENTSTATE state)
{
	if (action == NEXT)
	{
		if (state == E_DOWN)
		{
			if (active_cutscene != nullptr)
			{
				CS_Step* tmp = active_cutscene->GetInputStep();

				if (tmp)
					tmp->GetInput();
			}
		}
	}
}

bool j1CutSceneManager::PostUpdate()
{
	if (active_cutscene != nullptr)
	{
		active_cutscene->DrawElements(); //Draw external elements that aren't integrated in the main game  (Images, Texts...)
	}
	return true;
}

bool j1CutSceneManager::CleanUp()
{
	//Clear the cutscene (if it's being reproduced) 
	if (active_cutscene != nullptr)
	{
		//CLEAR STEPS & ELEMENTS OF THE CUTSCENE
		active_cutscene->ClearScene();
		RELEASE(active_cutscene);
	}

	//Clear paths list
	paths.clear();
	return true;
}

//Check if a cutscene is being reproduced
bool j1CutSceneManager::CutsceneReproducing() const
{
	if (active_cutscene != nullptr)
	{
		return true;
	}
	else
	{
		return false;
	}
}


//Read from .xml cutscene file
pugi::xml_node j1CutSceneManager::LoadXML(pugi::xml_document& config_file, std::string file) const
{
	pugi::xml_node ret;

	char* buf = nullptr;
	int size = App->fs->Load(file.c_str(), &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE(buf);

	if (result == NULL)
	{
		LOG("Could not load map xml file. Pugi error: %s", result.description());
	}
	else
	{
		ret = config_file.child("Cutscene");
	}

	return ret;
}

// --------------------------------------------------------



// CUTSCENE ----------------------------
uint Cutscene::GetID() const
{
	return id;
}

CS_Element* Cutscene::GetElement(const char* name)
{
	CS_Element* temp = nullptr;

	//Iterates elements list of the cutscene to find the correct element (this comparison can be done with the element id too)
	for (std::list<CS_Element*>::iterator it = elements.begin(); it != elements.end(); it++)
	{
		if (it._Ptr->_Myval->name == name)
		{
			temp = it._Ptr->_Myval;
			break;
		}
	}

	return temp;
}

bool Cutscene::isFinished() const
{
	return finished;
}

uint Cutscene::GetNumElements()
{
	return elements.size();
}

uint Cutscene::GetNumSteps()
{
	return steps.size();
}

CS_Step* Cutscene::GetInputStep()
{
	CS_Step* ret = nullptr;

	for (std::list<CS_Step*>::iterator it = steps.begin(); it != steps.end(); it++)
	{
		if ((*it)->isActive() && (*it)->isInput())
		{
			ret = (*it);
			break;
		}
	}

	return ret;
}

Cutscene::Cutscene()
{
}

Cutscene::~Cutscene()
{
	for (std::list<CS_Element*>::iterator it = elements.begin(); it != elements.end(); it++)
		RELEASE((*it));

	for (std::list<CS_Step*>::iterator it = steps.begin(); it != steps.end(); it++)
		RELEASE((*it));

	App->gui->EraseScreen(this->cutscene_screen);
}

bool Cutscene::Start()
{
	finished = false;
	timer.Start(); //Starts the timer to reproduce the steps in order.
	return true;
}

bool Cutscene::Update(float dt)
{
	bool ret = true;
	bool active = false;

	//Iterate the steps of the cutscene to update the active ones
	std::list<CS_Step*>::iterator temp = steps.begin();

	//UPDATE STEPS (MODIFY ELEMENTS) -------------------------
	while (temp != steps.end())
	{
		CS_Step* step = *temp;

		//TODO 7: Start the step if its start time has been reached (Use the cutscene timer to check the current time)
		//This function will be called only one time, so you will need 3 conditions: 
		// 1) if the step isn't active
		// 2) if the step isn't finished.
		// 3) if the start time has been reached by the cutscene timer
		

		if (temp != steps.begin() && temp._Ptr->_Prev->_Myval->isFinished() && temp._Ptr->_Prev->_Myval->isWait() && step->isActive() == false && step->isFinished() == false)
		{
			if (step->n == 1)
				int buenas_atrdes = 0;

			step->StartStep();
		}
		if (step->GetStartTime() != -1 && step->GetStartTime() <= timer.ReadSec() && step->isActive() == false && step->isFinished() == false)
			step->StartStep();

		//Update the active steps to perform its action
		if (step->isActive() == true)
		{
			step->DoAction(dt);

			if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
			{
				step->GetInput();
			}

		}
		temp++;
	}
	// ---------------------------

	//If all stpes have been reproduced, finish the cutscene
	if (steps_done >= num_steps)
	{
		finished = true;
	}

	return ret;
}

bool Cutscene::DrawElements()
{
	return true;
}

bool Cutscene::ClearScene()
{
	//CLEAR ELEMENTS
	for (std::list<CS_Element*>::iterator it = elements.begin(); it != elements.end(); it++)
	{
		RELEASE(it._Ptr->_Myval);
	}
	elements.clear();

	//CLEAR STEPS
	for (std::list<CS_Step*>::iterator it = steps.begin(); it != steps.end(); it++)
	{
		RELEASE(it._Ptr->_Myval);
	}
	steps.clear();

	return true;
}

bool Cutscene::LoadSceneName(pugi::xml_node& node)
{
	bool ret = false;

	if (node != NULL)
	{
		CS_Scene* tmp = new CS_Scene(CS_SCENE, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("active").as_bool(false), nullptr);

		if (tmp)
		{
			elements.push_back(tmp);
			tmp->scene_name = node.attribute("scene_name").as_string();
			ret = true;
		}

	}

	return ret;
}

bool Cutscene::LoadNPC(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		CS_npc* tmp = new CS_npc(CS_NPC, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("active").as_bool(false), nullptr);
		this->elements.push_back(tmp);

		Entity* tmp_ent = tmp->GetEntity(node.attribute("entity_id").as_int());
		tmp->empty = node.attribute("empty").as_bool();

		if (tmp->empty == false)
		{
			if (tmp_ent)
			{
				tmp->LinkEntity(tmp_ent);
				iPoint new_pos = { node.attribute("x").as_int(), node.attribute("y").as_int() };
				tmp_ent->MoveTo(new_pos.x, new_pos.y);
			}
			else
			{
				tmp->LinkEntity(App->game->em->CreateNPC(1, (NPC_TYPE)node.attribute("type").as_int(), node.attribute("x").as_int(), node.attribute("y").as_int(), node.attribute("entity_id").as_int()));
			}
		}
		else
		{
			//If is empty later will load, and need those variablos
			tmp->pos = { node.attribute("x").as_int(), node.attribute("y").as_int() };
			tmp->entity_id = node.attribute("entity_id").as_int();
			tmp->entity_type = node.attribute("type").as_int();
		}
		ret = true;
	}
	return ret;
}

bool Cutscene::LoadEnemy(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		CS_npc* tmp = new CS_npc(CS_ENEMY, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("active").as_bool(false), nullptr);
		this->elements.push_back(tmp);

		Entity* tmp_ent = tmp->GetEntity(node.attribute("entity_id").as_int());
		tmp->empty = node.attribute("empty").as_bool();

		if (tmp->empty == false)
		{
			if (tmp_ent)
			{
				tmp->LinkEntity(tmp_ent);
				iPoint new_pos = { node.attribute("x").as_int(), node.attribute("y").as_int() };
				tmp_ent->MoveTo(new_pos.x, new_pos.y);
			}
			else
			{
				tmp->LinkEntity((Entity*)App->game->em->CreateEnemy(1, (ENEMY_TYPE)node.attribute("type").as_int(), node.attribute("x").as_int(), node.attribute("y").as_int(), std::vector<iPoint>(), node.attribute("entity_id").as_int()));
			}
		}
		else
		{
			//If is empty later will load, and need those variablos
			tmp->pos = { node.attribute("x").as_int(), node.attribute("y").as_int() };
			tmp->entity_id = node.attribute("entity_id").as_int();
			tmp->entity_type = node.attribute("type").as_int();
		}
		ret = true;
	}
	return ret;

	return ret;
}

bool Cutscene::LoadImg(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		CS_Image* new_image = new CS_Image(CS_IMAGE, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("active").as_bool(false), node.attribute("file").as_string(""), node.attribute("text").as_string());

		if (new_image)
		{
			elements.push_back(new_image);
			new_image->img = (UI_Image*)App->game->hud->LoadUIElement(node, cutscene_screen, IMAGE);

			if (new_image->img)
				new_image->img->SetTexture((SDL_Texture*)App->gui->GetUITexture(new_image->texture_name));
			
			ret = true;
		}	
	}
	return false;
}

bool Cutscene::LoadCam(pugi::xml_node& node)
{
	bool ret = false;

	if (node != NULL)
	{
		CS_Element* new_cam = new CS_Element(CS_CAM, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("active").as_bool(false), node.attribute("file").as_string(""));

		if (new_cam)
		{
			elements.push_back(new_cam);
			ret = true;
		}
	}

	return ret;
}

bool Cutscene::LoadText(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		CS_Text* new_text = new CS_Text(CS_TEXT, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("active").as_bool(false), nullptr);
		
		if (new_text)
		{
			elements.push_back(new_text);
			new_text->SetString((UI_String*)App->game->hud->LoadUIElement(node, cutscene_screen, STRING));

			ret = true;
		}
	}
	return false;
}

bool Cutscene::LoadMusic(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		elements.push_back(new CS_Music(CS_MUSIC, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("active").as_bool(false), node.attribute("path").as_string("")));
		ret = true;
	}
	return false;
}

bool Cutscene::LoadFx(pugi::xml_node& node)
{
	bool ret = false;
	if (node != NULL)
	{
		elements.push_back(new CS_SoundFx(CS_FX, node.attribute("n").as_int(-1), node.attribute("name").as_string(""), node.attribute("active").as_bool(false), node.attribute("path").as_string(""), node.attribute("loops").as_uint()));
		ret = true;
	}
	return false;
}

bool Cutscene::LoadStep(pugi::xml_node& node, Cutscene* cutscene) //Pass the cutscene that it's involved to link the step with it
{
	bool ret = false;
	if (node != NULL && cutscene != nullptr)
	{
		CS_Step* temp_step = new CS_Step(node.attribute("n").as_int(-1), node.attribute("start").as_float(-1), node.attribute("duration").as_float(-1), node.attribute("input").as_bool(), cutscene);

		if (node.attribute("duration").as_int() == -1)
			temp_step->SetWait(true);

		//Link to the correct element of the cutscene
		temp_step->SetElement(node);

		//Set the action that will be applied to the linked element
		temp_step->SetAction(node);

		this->steps.push_back(temp_step);
		num_steps++;
		ret = true;
	}
	return ret;
}

void Cutscene::StepDone()
{
	steps_done++;
	
}

//Assign a map id to load it after the cutscene
bool Cutscene::SetMap(pugi::xml_node& node)
{
	map_id = node.child("MAP").attribute("id").as_int(-1); //if it has no MAP id, it's set to -1
	return false;
}

//--------------------------------------


//CS ELEMENTS ------------------------------
CS_Element::CS_Element(CS_Type type, int n, const char* name, bool active, const char* path) :type(type), n(n), active(active), name(name)
{
	if (path != nullptr)
	{
		this->path = path;
	}
}

CS_Element::~CS_Element()
{
}

CS_Type CS_Element::GetType() const
{
	return type;
}


//CS STEPS ----------------------------------
CS_Step::CS_Step(int n, float start, float duration, bool input, Cutscene* cutscene) :n(n), start(start), duration(duration),input(input), cutscene(cutscene)
{
}

CS_Step::~CS_Step()
{
}

bool CS_Step::DoAction(float dt)
{
	std::string action_name;

	//TODO 8: In each act_type case, add the correct function that will perform the desired action.
	//Depending on the action type, a different function will be called
	switch (act_type)
	{
	case ACT_DISABLE:
		action_name = "disable";
		DisableElement();
		break;

	case ACT_ENABLE:
		action_name = "enable";
		ActiveElement();
		break;

	case ACT_MOVE:
		action_name = "move";
		DoMovement(dt);
		break;

	case ACT_PLAY:
		action_name = "play";
		this->Play();
		break;
	case ACT_STOP:
		action_name = "stop";
		this->StopMusic();
		break;
		
	case ACT_SET_STRING:
		action_name = "setstring";
		ChangeString();

	case ACT_FADE:
		action_name = "fade";
		Fade();
		break;

	case ACT_LOAD:
		action_name = "load";
		LoadScene();
		break;

	case ACT_CREATE:
		action_name = "create";
		CreateCharacter();
		break;

	case ACT_FREE_CAM:
		action_name = "free";
		FreeCamera();
		break;

	case ACT_LOCK_CAM:
		action_name = "lock";
		LockCamera();
		break;

	case ACT_CHANGE_AGE:
		action_name = "change age";
		ChangeAge();
		break;

	case ACT_FADE_CAM:
		action_name = "fading_to_black";
		Fade();
		break;

	case ACT_UNFADE_CAM:
		action_name = "fading_from_black";
		Fade();
		break;

	default:
		action_name = "none";
		break;
	}

	//Finish the current step if its duration has reach the limit (if the step works with TIME, without taking care of the position)
	if (duration > -1 && cutscene->timer.ReadSec() >= start + duration)
	{
		FinishStep();
	}
	return true;
}

CS_Type CS_Step::GetElementType() const
{
	return element->GetType();
}

uint CS_Step::GetStartTime() const
{
	return start;
}

void CS_Step::StartStep()
{
	active = true;
	LOG("Step %i started at %.3fs", n, cutscene->timer.ReadSec());
}

void CS_Step::FinishStep()
{
	active = false;
	finished = true;
	
	if (act_type == ACT_SET_STRING)
	{
		CS_Text* tmp = (CS_Text*)element;
		tmp->GetText()->Set_Active_state(false);
		tmp->Changed_string = false;
	}

	cutscene->StepDone(); //Increment the "steps done" counter
	LOG("Step %i finished at %.3fs", n, cutscene->timer.ReadSec());
}

//Set the action programmed to that step that will execute the linked element
void CS_Step::SetAction(pugi::xml_node& node)
{
	std::string action_type = node.child("element").attribute("action").as_string("");

	if (action_type == "enable")
	{
		act_type = ACT_ENABLE;
	}
	else if (action_type == "disable")
	{
		act_type = ACT_DISABLE;
	}
	else if (action_type == "move")
	{
		act_type = ACT_MOVE;
		iPoint destination = { node.child("element").child("movement").attribute("dest_x").as_int(0), node.child("element").child("movement").attribute("dest_y").as_int(0) };
		std::string direction_type = node.child("element").attribute("dir").as_string("");

		bezier_time = node.child("element").child("movement").attribute("bezier_time").as_int();

		//Load the movement variables (origin, destination, speed, direction)
		LoadMovement(destination, node.child("element").child("movement").attribute("speed").as_int(1), direction_type);
	}
	else if (action_type == "play")
	{
		act_type = ACT_PLAY;
	}
	else if (action_type == "stop")
	{
		act_type = ACT_STOP;
	}
	else if (action_type == "setstring")
	{
		act_type = ACT_SET_STRING;
		new_text = node.child("element").child("text").attribute("text").as_string();
	}
	else if (action_type == "fade")
	{
		act_type = ACT_FADE;
		fade_black = node.child("element").child("fading").attribute("to_black").as_bool();
		bezier_time = node.child("element").child("fading").attribute("bezier_time").as_int();

		if (fade_black)
			dynamic_cast<CS_Image*>(element)->img->alpha = 0;
	}
	else if(action_type == "load")
	{
		act_type = ACT_LOAD;
	}
	else if (action_type == "create")
	{
		act_type = ACT_CREATE;
	}
	else if (action_type == "free")
	{
		act_type = ACT_FREE_CAM;
	}
	else if (action_type == "lock")
	{
		act_type = ACT_LOCK_CAM;
	} 
	else if (action_type == "setage")
	{
		act_type = ACT_CHANGE_AGE;
		LoadCharacter(node.child("element").child("age"));
	}
	else if (action_type == "fade_cam")
	{
		act_type = ACT_FADE_CAM;
		bezier_time = node.child("element").child("fading").attribute("bezier_time").as_int();
	}
	else if (action_type == "unfade_cam")
	{
		act_type = ACT_UNFADE_CAM;
		bezier_time = node.child("element").child("fading").attribute("bezier_time").as_int();
	}
	else
	{
		act_type = ACT_NONE;
	}
}

void CS_Step::LoadMovement(iPoint destination, int speed, const std::string& dir)
{

	if (dir == "up")
	{
		direction = CS_UP;
	}
	else if (dir == "down")
	{
		direction = CS_DOWN;
	}
	else if (dir == "left")
	{
		direction = CS_LEFT;
	}
	else if (dir == "right")
	{
		direction = CS_RIGHT;
	}
	else if (dir == "teleport")
	{
		direction = CS_TELEPORT;
	}
	else
	{
		direction = NO_DIR;
	}

	//Set the same origin as the linked element
	origin = element->GetPos();

	//Set the destination that will reach the linked element
	if (bezier_time == 0)
	{
		if (direction == CS_TELEPORT)
			dest = App->map->GetTileCenter(destination);
		else dest = destination;
	}
	else
	{
		dest = origin + destination;
	}
	

	//Set the movement speed
	mov_speed = speed;

	LOG("Movement Loaded-> oX:%i oY:%i dX:%i dY:%i speed:%i dir:%i", origin.x, origin.y, dest.x, dest.y, speed, direction);

}

void CS_Step::LoadScene()
{
	if (!loading)
	{
		CS_Scene* tmp = (CS_Scene*)element;
		const char* lol = tmp->scene_name.c_str();
		App->sceneM->RequestSceneChange({ 0,0 }, lol, D_DOWN);
		loading = true;
	}

	if (App->sceneM->currentScene)
	{
		if (App->sceneM->currentScene->name == dynamic_cast<CS_Scene*>(element)->scene_name)
		{
			FinishStep();
			loading = false;
		}
	}
		
}

void CS_Step::LoadCharacter(pugi::xml_node& node)
{
	string tmp = node.attribute("state").as_string();

	if (tmp == "young")
	{
		age = YOUNG;
		return;
	}
		
	if (tmp == "adult")
	{
		age = ADULT;
		return;
	}
}

void CS_Step::CreateCharacter()
{
	if (GetElementType() == CS_NPC)
	{
		CS_npc* tmp = (CS_npc*)element;

		if (tmp->entity_id != -1)
		{
			Entity* ent = App->game->em->GetEntityFromId(tmp->entity_id);

			if (ent)
			{
				tmp->LinkEntity(ent);
				tmp->GetMyEntity()->MoveToTile(tmp->pos.x, tmp->pos.y);
			}
			else tmp->LinkEntity(App->game->em->CreateNPC(1, (NPC_TYPE)tmp->entity_type, tmp->pos.x, tmp->pos.y, tmp->entity_id));
			
		}
		else
		{
			if (App->game->em->player)
			{
				tmp->LinkEntity(App->game->em->player);
				App->game->em->player->MoveToTile(tmp->pos.x, tmp->pos.y);
			}
			else tmp->LinkEntity(App->game->em->CreatePlayer(tmp->pos.x, tmp->pos.y, YOUNG));
			
		}

		tmp->empty = false;
		FinishStep();
		return;
	}

	if (GetElementType() == CS_ENEMY)
	{
		CS_npc* tmp = (CS_npc*)element;

		if (tmp->entity_id != -1)
		{
			Entity* ent = App->game->em->GetEntityFromId(tmp->entity_id);

			if (ent)
			{
				tmp->LinkEntity(ent);
				tmp->GetMyEntity()->MoveToTile(tmp->pos.x, tmp->pos.y);
			}
			else tmp->LinkEntity((Entity*)App->game->em->CreateEnemy(1, (ENEMY_TYPE)tmp->entity_type, tmp->pos.x, tmp->pos.y,vector<iPoint>(), tmp->entity_id));

		}
		tmp->empty = false;
		FinishStep();
		return;
	}

	return;
}

bool CS_Step::DoMovement(float dt)
{
	iPoint curr_pos;

	//Switch case for different directions of movement
	if (element->GetType() == CS_IMAGE)
	{
		CS_Image* image = static_cast<CS_Image*>(element);
		switch (direction)
		{
		case CS_UP:
			if (bezier_active == false)
			{
				image->img->SetAnimationTransition(ANIMATION_TRANSITION::T_MOVE_UP, bezier_time, this->dest);
				bezier_active = true;
			}
			break;
		case CS_DOWN:
			if (bezier_active == false)
			{
				image->img->SetAnimationTransition(ANIMATION_TRANSITION::T_MOVE_DOWN, bezier_time, this->dest);
				bezier_active = true;
			}
			break;
		case CS_LEFT:
			if (bezier_active == false)
			{
				image->img->SetAnimationTransition(ANIMATION_TRANSITION::T_MOVE_LEFT, bezier_time, this->dest);
				bezier_active = true;
			}
			break;
		case CS_RIGHT:
			if (bezier_active == false)
			{
				image->img->SetAnimationTransition(ANIMATION_TRANSITION::T_MOVE_RIGHT, bezier_time, this->dest);
				bezier_active = true;
			}
			break;
		default:
			break;
		}
		curr_pos = image->GetPos();
	}

	if (element->GetType() == CS_NPC)
	{
		CS_npc* tmp = (CS_npc*)element;
		switch (direction)
		{
		case CS_UP:
			tmp->Move(0, -ceil(mov_speed*dt));
			tmp->GetMyEntity()->currentDir = DIRECTION::D_UP;
			break;
		case CS_DOWN:
			tmp->Move(0, ceil(mov_speed*dt));
			tmp->GetMyEntity()->currentDir = DIRECTION::D_DOWN;
			break;
		case CS_LEFT:
			tmp->Move(-ceil(mov_speed*dt), 0);
			tmp->GetMyEntity()->currentDir = DIRECTION::D_LEFT;
			break;
		case CS_RIGHT:
			tmp->Move(ceil(mov_speed*dt), 0);
			tmp->GetMyEntity()->currentDir = DIRECTION::D_RIGHT;
			break;

		case CS_TELEPORT:
			if (tmp->GetMyEntity())
			{
				tmp->GetMyEntity()->currentPos = { dest.x, dest.y };
			}
			break;

		case NO_DIR:
			break;
		default:
			break;
		}
		curr_pos = element->GetPos();
	}

	//Check if the action is completed to finish the step
	CheckMovementCompleted(curr_pos);

	LOG("Step %i Moving %s X:%i Y:%i", n, element->name.c_str(), curr_pos.x, curr_pos.y);

	return true;
}

bool CS_Step::CheckMovementCompleted(iPoint curr_pos)
{
	bool ret = false;

	if (bezier_time)
	{
		if (element->GetType() == CS_IMAGE)
		{
			if (dynamic_cast<CS_Image*>(element)->img->GetCurrentTransition() == NO_AT && input == false)
			{
				FinishStep();
				bezier_active = false;
				return true;
			}
		}
	}
	else
	{
		switch (direction)
		{
		case CS_UP:
			if (App->map->WorldToMap(curr_pos.x, curr_pos.y).y <= dest.y)
			{
				ret = true;
				FinishStep();
			}
			break;
		case CS_DOWN:
			if (App->map->WorldToMap(curr_pos.x, curr_pos.y).y >= dest.y)
			{
				ret = true;
				FinishStep();
			}
			break;
		case CS_LEFT:
			if (App->map->WorldToMap(curr_pos.x, curr_pos.y).x <= dest.x)
			{
				ret = true;
				FinishStep();
			}
			break;
		case CS_RIGHT:
			if (App->map->WorldToMap(curr_pos.x, curr_pos.y).x >= dest.x)
			{
				ret = true;
				FinishStep();
			}
			break;
		case CS_TELEPORT:
			ret = true;
			FinishStep();
			break;
		default:
			break;
		}
	}
	return ret;
}

void CS_Step::Play()
{
	//Differentiation between Play Music / Play SoundFx
	if (element->GetType() == CS_MUSIC)
	{
		CS_Music* mus = static_cast<CS_Music*>(element);
		mus->Play();
	}
	if (element->GetType() == CS_FX)
	{
		CS_SoundFx* fx = static_cast<CS_SoundFx*>(element);
		fx->Play();
	}
	LOG("Step %i Playing %s", n, element->name.c_str());
}

void CS_Step::ChangeString()
{
	if (element->GetType() == CS_TEXT)
	{
		CS_Text* tmp = (CS_Text*)element;

		if (!tmp->Changed_string)
		{
			tmp->GetText()->Set_Active_state(true);
			tmp->GetText()->Set_String((char*)this->new_text.c_str());
			tmp->Changed_string = true;
		}

		if (element->GetType() == CS_TEXT && input == false)
		{
			if (dynamic_cast<CS_Text*>(element)->GetText()->dialog_state == FINISHED_TEXT)
				FinishStep();
		}
	}

}

void CS_Step::StopMusic()
{
	if (element->GetType() == CS_MUSIC)
	{
		//App->audio->StopMusic();
		LOG("Step %i Stoping %s", n, element->name.c_str());
	}
}

void CS_Step::ActiveElement()
{
	if (element->active == false)
	{
		element->active = true;

		if (element->GetType() == CS_TEXT)
		{
			CS_Text* tmp = (CS_Text*)element;
			tmp->GetText()->Set_Active_state(true);

		}

		if (element->GetType() == CS_IMAGE)
		{
			CS_Image* tmp = (CS_Image*)element;
			tmp->img->Set_Active_state(true);

		}

		//Means that when doned active it
		if (duration == -1 && input == false)
			FinishStep();

		LOG("Step %i Enabling %s", n, element->name.c_str());
	}

	if (element->GetType() == CS_TEXT)
	{
		if(dynamic_cast<CS_Text*>(element)->GetText()->dialog_state == FINISHED_TEXT && !input)
			FinishStep();
	}

}

void CS_Step::DisableElement()
{
	if (element->active == true)
	{
		element->active = false;
		LOG("Step %i Disabling %s", n, element->name.c_str());
	}

	if (element->GetType() == CS_TEXT)
	{

		CS_Text* tmp = (CS_Text*)element;
		tmp->GetText()->Set_Active_state(false);
		//element->active = false;
		LOG("Step %i Disabling %s", n, element->name.c_str());
		FinishStep();
	}
}

void CS_Step::FreeCamera()
{
	if (element->GetType() == CS_CAM)
	{
		App->render->freeCamera = true;
		FinishStep();
	}
	
}

void CS_Step::LockCamera()
{
	if (element->GetType() == CS_CAM)
	{
		App->render->freeCamera = false;
		FinishStep();
	}
}

void CS_Step::Fade()
{
	if (act_type == ACT_FADE)
	{
		if (element->GetType() == CS_IMAGE)
		{

			CS_Image* tmp = (CS_Image*)element;

			if (bezier_active == false)
			{
				if (tmp->img->active == false)
					tmp->img->Set_Active_state(true);

				tmp->img->SetAnimationTransition(T_FADE_TO, bezier_time, iPoint(0, 255));
				bezier_active = true;
			}
			CheckFadeCompleted();
			return;
		}
	}
	
	if (act_type == ACT_FADE_CAM)
	{
		if (bezier_active == false)
		{
			this->bezier_active = true;
			App->render->IntoFade();
			App->render->fade_timer.Start();
			
		}
		else
		{
			int current_time = App->render->fade_timer.Read();
			if (current_time <= bezier_time)
			{
				float value = App->render->fade_bezier->GetActualX(bezier_time, current_time, cbezier_type::CB_LINEAL);
				value = 255 * CLAMP01(value);
				App->render->SetAlpha((255 * value));
			}
			else
			{
				bezier_active = false;
				FinishStep();
			}
		}
		return;
	}

	if (act_type == ACT_UNFADE_CAM)
	{
		if (!bezier_active)
		{
			App->render->fade_timer.Start();
			bezier_active = true;
		}
		else
		{
			int current_time = App->render->fade_timer.Read();
			if (current_time <= bezier_time)
			{
				float value = App->render->fade_bezier->GetActualX(bezier_time, current_time, cbezier_type::CB_LINEAL);
				value = CLAMP01(value);
				int alpha = (255 * (1 - value));
				App->render->SetAlpha(alpha);

			}
			else
			{
				bezier_active = false;
				App->render->StopFade();
				FinishStep();
			}
		}
		return;
	}


}

bool CS_Step::CheckFadeCompleted()
{
	if (bezier_time)
	{
		if (dynamic_cast<CS_Image*>(element)->img->GetCurrentTransition() == NO_AT && input == false)
		{
			FinishStep();
			bezier_active = false;
		}
	}

	return false;
}

bool CS_Step::ChangeAge()
{
	bool ret = false;

	if (element->GetType() == CS_NPC)
	{
		CS_npc* tmp = (CS_npc*)element;

		if (tmp->GetMyEntity() != nullptr)
		{
			if (tmp->GetMyEntity() == App->game->em->player)
			{
				App->game->em->player->ChangeAge(age);
				App->game->em->player->life = App->game->em->player->maxLife;
				FinishStep();
				ret = true;
			}
		}

	}
	return ret;
}

//Link the element of the cutscene with the step
void CS_Step::SetElement(pugi::xml_node& node)
{
	std::string element_name = node.child("element").attribute("name").as_string("");
	element = cutscene->GetElement(element_name.c_str());
}

bool CS_Step::isActive() const
{
	return active;
}

bool CS_Step::isFinished() const
{
	return finished;
}

bool CS_Step::isWait() const
{
	return wait_prev_step;
}

bool CS_Step::isInput() const
{
	return input;
}

void CS_Step::SetWait(bool wait)
{
	wait_prev_step = wait;
}

void CS_Step::GetInput()
{
	if (element->GetType() == CS_TEXT)
	{
		if (dynamic_cast<CS_Text*>(element)->GetText()->dialog_state != FINISHED_TEXT)
			dynamic_cast<CS_Text*>(element)->GetText()->ForcedFinish();
		
		else FinishStep();
	}

	if (element->GetType() == CS_IMAGE)
	{	
		if(act_type == ACT_FADE)
		{
			if (dynamic_cast<CS_Image*>(element)->img->GetCurrentTransition() != NO_AT)
				dynamic_cast<CS_Image*>(element)->img->ForcedFinishTransition();
			else FinishStep();
		}	
	}

}

// CS IMAGE -----------------
CS_Image::CS_Image(CS_Type type, int n, const char* name, bool active, const char* path, const char* texture) : CS_Element(type, n, name, active, path), texture_name(texture) {}

CS_Image::~CS_Image()
{
}

void CS_Image::Move(float x, float y)
{


	//pos.x += x;
	//pos.y += y;
}

iPoint CS_Image::GetPos()
{
	return { img->Interactive_box.x, img->Interactive_box.y };
}

//-----------------------------

//CS MUSIC --------------------
CS_Music::CS_Music(CS_Type type, int n, const char* name, bool active, const char * path) :
	CS_Element(type, n, name, active, path)
{
}

CS_Music::~CS_Music()
{
}

void CS_Music::Play()
{
	App->audio->PlayMusic(path.c_str());
}
//----------------------------------------


//CS FX ----------------------------------------
CS_SoundFx::CS_SoundFx(CS_Type type, int n, const char* name, bool active, const char* path, uint loops) :
	CS_Element(type, n, name, active, path), loops(loops)
{
	if (path != nullptr)
	{
		LoadFx(); //Load the sound effect into the Audio Module
	}
}

CS_SoundFx::~CS_SoundFx()
{
}

void CS_SoundFx::LoadFx()
{
	//fx_id = App->audio->LoadFx(path.c_str());
}

void CS_SoundFx::Play()
{
	//App->audio->PlayFx(fx_id, loops);
}

uint CS_SoundFx::GetID() const
{
	return fx_id;
}

uint CS_SoundFx::GetLoops() const
{
	return loops;
}

// ---------------------------------------

// CS TEXT --------------------------------------
CS_Text::CS_Text(CS_Type type, int n, const char* name, bool active, const char* path) :
	CS_Element(type, n, name, active, path)
{}

CS_Text::~CS_Text()
{
}

void CS_Text::SetPos(int x, int y)
{
	this->text->Set_Interactive_Box({ x,y,390,0 });
}

void CS_Text::SetString(UI_String* ui_string)
{
	text = ui_string;
	return;
}

void CS_Text::SetText(const char* txt)
{
	text->Set_String((char*)txt);
}

UI_String* CS_Text::GetText() const
{
	return text;
}
// -----------------------------------------------


//CS NPC ------------------------------------------------
CS_npc::CS_npc(CS_Type type, int n, const char* name, bool active, const char* path) :
	CS_Element(type, n, name, active, path)
{
}

CS_npc::~CS_npc()
{
}

Entity* CS_npc::GetEntity(uint id) const
{
	if (id == -1)
	{
		if (Entity* tmp = App->game->em->player)
			return tmp;
		else return App->game->em->player = App->game->em->CreatePlayer(0, 0, LINK_AGE::YOUNG);
	}

	if (Entity* tmp = App->game->em->GetEntityFromId(id))
		return tmp;

	else 
	{
		//Create the entity
		//App->game->em->CreateNPC()
	}


	return nullptr;
}

Entity* CS_npc::GetMyEntity() const
{
	return entity;
}

void CS_npc::LinkEntity(Entity* e)
{
	entity = e; //Set the pointer to the entity of the game to take control of it
}

void CS_npc::Move(float x, float y)
{
	entity->currentPos.x += x;
	entity->currentPos.y += y;
}

iPoint CS_npc::GetPos()
{
	if (empty)
		return App->map->GetTileCenter(pos);
	else return entity ? entity->currentPos : iPoint(0,0);
}
// ------------------------------------------------------

CS_Scene::CS_Scene(CS_Type type, int n, const char * name, bool active, const char * path) : CS_Element(type, n, name, active, path)
{
}

CS_Scene::~CS_Scene()
{
}
