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

class TextLine
{
public:

	TextLine(int state, std::string text);
	~TextLine();

	int state;
	std::string* line = nullptr;
};

class Dialog
{
public:

	Dialog(int id);
	~Dialog();

	uint id;
	std::vector<TextLine*> texts;
};

class DialogManager : public j1Module
{
public:

	DialogManager();
	~DialogManager();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool BlitDialog(int id, int state);

	int dialogueStep = 0; //Allows to order the conversarion correctly

	/*--- UI elements to print dialogues on screen ---*/
	UI_element* screen = nullptr;
	UI_String* text_on_screen = nullptr;
	/*-- END --*/

private:
	std::vector<Dialog*> dialog;

	/*-- Data to load XML --*/
	std::string folder;
	std::string path;
	pugi::xml_document dialogueDataFile;
	pugi::xml_node dialogueNode;
	/*-- END --*/
};

#endif