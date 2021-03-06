#ifndef _CUTSCENE_MANAGER_H_
#define _CUTSCENE_MANAGER_H_

#include "j1Module.h"
#include "InputManager.h"
#include "Bezier.h"
#include <vector>
#include <string>


enum CS_Type { CS_IMAGE, CS_TEXT, CS_NPC, CS_ENEMY, CS_DYNOBJECT, CS_ITEM, CS_MUSIC, CS_FX, CS_SCENE, CS_CAM, CS_NONE };
enum Action_Type { ACT_ENABLE, ACT_DISABLE, ACT_LOAD, ACT_SET_STRING, ACT_MOVE, ACT_FADE, ACT_FADE_CAM, ACT_UNFADE_CAM, ACT_CREATE, ACT_PLAY, ACT_STOP, ACT_FREE_CAM, ACT_LOCK_CAM, ACT_CHANGE_AGE, ACT_ZELDA_TELEPORT, ACT_NONE };
enum Dir_Type { CS_UP, CS_DOWN, CS_LEFT, CS_RIGHT, CS_TELEPORT, NO_DIR };

enum LINK_AGE;

class Entity;
class j1Timer;
class Cutscene;
class UI_String;
class UI_Image;

//Base class for elements of the cutscene
class CS_Element
{
public:
	CS_Element() {}
	CS_Element(CS_Type type, int n, const char* name, bool active, const char* path);
	virtual ~CS_Element();

	//UTILITY FUNCTIONS ---------
	CS_Type GetType() const;
	virtual iPoint GetPos() { return iPoint(0, 0); };
	// ------------------------

	std::string name;
	bool active = false;

protected:
	CS_Type type = CS_NONE;		//Cutscene element type
	int n = -1;					//identifier
	std::string path;			//auxiliar path (texture file name, animation, sound/music file...)
};

class CS_npc : public CS_Element
{
public:
	CS_npc(CS_Type type, int n, const char* name, bool active, const char* path);
	~CS_npc();

	Entity* GetEntity(uint id) const;
	Entity* GetMyEntity() const;
	void LinkEntity(Entity* e);

	void Move(float x, float y);
	iPoint GetPos();
	bool empty = false;

	//If empty
	iPoint pos = { 0,0 };
	int entity_id = 0;
	int entity_type = -1;

private:

	Entity*	entity = nullptr;	//Pointer to the entity that forms part of the game elements (to modify its state, do actions, etc)
};

class CS_Image : public CS_Element
{
public:
	CS_Image(CS_Type type, int n, const char* name, bool active, const char* path, const char* texture);
	~CS_Image();
	void Move(float x, float y);

	//UTILITY FUNCTIONS -------------
	string	texture_name;
	iPoint GetPos();
	//-------------------------------

public:
	UI_Image* img = nullptr;
};

class CS_Text : public CS_Element
{
public:
	CS_Text(CS_Type type, int n, const char* name, bool active, const char* path);
	~CS_Text();

	//UTILITY FUNCTIONS ---------------
	void SetText(const char* txt);
	UI_String* GetText()const;
	void SetPos(int, int);
	void SetString(UI_String*);
	bool Changed_string = false;

	//void Move(float x, float y);
	//---------------------------------

private:
	UI_String* text = nullptr;
};

class CS_Music : public CS_Element
{
public:
	CS_Music(CS_Type type, int n, const char* name, bool active, const char* path);
	~CS_Music();

	//UTILITY FUNCTIONS ------------
	void Play();
	//------------------------------
};

class CS_SoundFx : public CS_Element
{
public:
	CS_SoundFx(CS_Type type, int n, const char* name, bool active, const char* path, uint loops);
	~CS_SoundFx();

	//UTILITY FUNCTIONS ------------
	void LoadFx();
	void Play();
	uint GetID() const;
	uint GetLoops() const;
	//------------------------------

private:
	uint fx_id = 0;
	uint loops = 0;
};

class CS_Scene : public CS_Element
{
public:
	CS_Scene(CS_Type type, int n, const char* name, bool active, const char* path);
	~CS_Scene();

public:

	string scene_name;
};

class CS_Step
{
public:
	CS_Step(int n, float start, float duration, bool input, Cutscene* cutscene);
	virtual ~CS_Step();

	//Perform the correct action according to the action type assigned
	bool DoAction(float dt);

	//STEP FUNCTIONS -------------
	void StartStep();
	void FinishStep();
	void SetElement(pugi::xml_node&);
	void SetAction(pugi::xml_node&);
	//------------------------------

	//ACTION FUNCTIONS ----------
	void LoadMovement(iPoint dest, int speed, const std::string& dir);
	void LoadScene();
	void LoadCharacter(pugi::xml_node&);
	void CreateCharacter();
	bool DoMovement(float dt);
	bool CheckMovementCompleted(iPoint curr_pos);
	void Play();
	void ChangeString();
	void StopMusic();
	void ActiveElement();
	void DisableElement();
	void FreeCamera();
	void LockCamera();
	void Fade();
	bool CheckFadeCompleted();
	bool ChangeAge();

	//visente for GOLD
	void ZeldaAnimation();

	//---------------------------

	//UTILITY FUNCTIONS ------------
	CS_Type GetElementType() const;
	uint GetStartTime() const;
	bool isActive() const;
	bool isFinished() const;
	bool isWait() const;
	bool isInput() const;
	void SetWait(bool);
	void GetInput();
	// --------------------------------

	int n = -1;

private:
	Cutscene*	cutscene = nullptr;		//Pointer to the cutscene that it is integrated
	//int n = -1;							//Number identifier to manage an order
	float		start = -1;					//Time to start the step
	float		duration = -1;				//Duration of the step
	int			bezier_time = 0;
	bool		input = false;				//If this step requieres an input to finish
	Action_Type act_type = ACT_NONE;		//Type of action that will be executed in this step
	CS_Element*	element = nullptr;		//Element to apply the action

	bool active = false;				//If step is reproducing.
	bool finished = false;
	bool wait_prev_step = false;

	//ACTIONS VARIABLES
	/*Movement*/
	iPoint origin = { 0, 0 };
	iPoint dest = { 0, 0 };
	int mov_speed = 0;
	Dir_Type direction = NO_DIR;
	bool	bezier_active = false;
	bool	fade_black = false;
	
	//Load functionality
	bool loading = false;

	//Character things
	iPoint pos = { 0,0 };
	int type = 0;
	int id = 0;

	LINK_AGE age = (LINK_AGE)0;

	//TEXTS ACIONS 
	std::string new_text;

};

class Cutscene
{
public:
	Cutscene();
	~Cutscene();

	bool Start();
	bool Update(float dt);
	bool DrawElements();
	bool ClearScene();

	//LOAD ELEMENTS FUNCTIONS -------
	bool LoadSceneName(pugi::xml_node&);
	bool LoadNPC(pugi::xml_node&);
	bool LoadEnemy(pugi::xml_node&);
	bool LoadImg(pugi::xml_node&);
	bool LoadCam(pugi::xml_node&);
	bool LoadText(pugi::xml_node&);
	bool LoadMusic(pugi::xml_node&);
	bool LoadFx(pugi::xml_node&);
	// ------------------------------

	//STEPS FUNCTIONS -------
	bool LoadStep(pugi::xml_node&, Cutscene* cutscene);
	void StepDone();
	//-----------------------

	//MAP ----------------------
	bool SetMap(pugi::xml_node&);
	//--------------------------

	//UTILITY FUNCTIONS ----------
	uint GetID() const;
	CS_Element* GetElement(const char* name);
	bool isFinished() const;
	uint GetNumElements();
	uint GetNumSteps();
	CS_Step* GetInputStep();
	//----------------------------

	std::string name;						//Name of the cutscenes
	uint id = 0;							//ID to locate when triggered
	j1Timer	timer;							//To control reproducing time of the cutscene
	int map_id = -1;						//Id to know wich map charge
	UI_element* cutscene_screen = nullptr;	//For ui cutscene elements

private:
	std::list<CS_Element*> elements;		//Elements controlled by the cutscene
	std::list<CS_Step*> steps;				//Steps to follow in order when reproduced
	bool finished = false;					//To know if Cutscene has finished
											//TO CONTROL WHEN THE CUTSCENE IS FINISHED
	uint num_steps = 0;
	uint steps_done = 0;
};


class j1CutSceneManager : public j1Module, public InputListener
{
public:
	j1CutSceneManager();
	~j1CutSceneManager();

	// Called when before render is available
	bool Awake(pugi::xml_node&);
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	//CUTSCENES MANAGEMENT ------
	bool LoadCutscene(uint id);
	bool StartCutscene(uint id);
	bool FinishCutscene();
	//---------------------------

	void OnInputCallback(INPUTEVENT action, EVENTSTATE state);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//Check if a cutscene is being reproduced
	bool CutsceneReproducing() const;

private:

	pugi::xml_node LoadXML(pugi::xml_document& config_file, std::string file) const;

	std::list<std::string> paths;			//Container with names of all paths of the cutscenes (in order)
	Cutscene* active_cutscene = nullptr;	//To know wich cutscene is active
};



#endif
