#ifndef __DIALOG_MANAGER_H__
#define __DIALOG_MANAGER_H__


#include "PugiXml\src\pugixml.hpp"
#include <string>
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1FileSystem.h"
#include "j1GameLayer.h"
#include "j1App.h"
#include "UI_String.h"

class Entity;
class Player;
class Enemy;
class UI_element;
class UI_String;
enum ENTITY_TYPE;
enum ENEMY_TYPE;

class Line
{
public:

	Line(int state, std::string text);
	~Line();

	int state;
	std::string* line = nullptr;
};

class Dialog
{
public:

	Dialog(int id);
	~Dialog();

	int id;
	std::vector<Line*> texts;
};

class DialogManager : public j1Module
{
public:

	DialogManager();
	~DialogManager();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PostUpdate();
	bool BlitDialog(int id, int state);

private:

	int dialogState = 0;

	std::string folder;
	std::string path;
	pugi::xml_document dialogDataFile;
	pugi::xml_node dialogNode;

	UI_element* screen = nullptr;
	UI_String* text_on_screen = nullptr;

	std::vector<Dialog*> dialog;

	/*---CODE TO TEST IN-GAME RESULTS ---*/
	int id = 1;
	int stateInput = 0;
	/*--- END ---*/
};

#endif