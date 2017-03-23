#ifndef __DIALOG_MANAGER_H__
#define __DIALOG_MANAGER_H__


#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <map>
#include <string>
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1FileSystem.h"
#include "j1GameLayer.h"
#include "j1App.h"


class Entity;
class Player;
class Enemy;
enum ENTITY_TYPE;
enum ENEMY_TYPE;

class Line
{
public:

	Line(bool interaction, std::string text);
	~Line();

	bool interaction;
	std::string* line = nullptr;
};

class Dialog
{
private:
public:

	Dialog(int id, int state);
	Dialog();
	~Dialog();

	int id;
	int state;
	std::vector<Line*> texts;
};

class DialogManager : public j1Module
{
public:

	DialogManager();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);

	bool DialogCharge(const int id);

public:
	std::string folder;
	std::string path;
	pugi::xml_document dialogDataFile;
	pugi::xml_node dialogNode;

	std::vector<Dialog*> dialog;
};

#endif