#include "NPC.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1CollisionManager.h"
#include "p2Log.h"
#include "Animation.h"
#include "InputManager.h"
#include "j1PerfTimer.h"

Npc::Npc() : Entity(), dialogState(0) { App->inputM->AddListener(this); }

bool Npc::Spawn(std::string file, iPoint pos, NPC_TYPE type)
{

	bool ret = true;

	// set position
	currentPos = lastPos = pos;

	// load xml attributes
	pugi::xml_document	attributesFile;
	char* buff;
	int size = App->fs->Load(file.c_str(), &buff);
	pugi::xml_parse_result result = attributesFile.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load attributes xml file. Pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		pugi::xml_node attributes;

		switch (type)
		{
		case NPC_1:
			attributes = attributesFile.child("attributes").child("npc1");
			break;
		}
		
		actionState = IDLE;
		LoadAttributes(attributes);
	}

	return ret;
}

bool Npc::Update(float dt)
{
	switch (actionState)
	{
	case IDLE:
			break;
	}
	return false;
}

void Npc::OnDeath()
{

}
