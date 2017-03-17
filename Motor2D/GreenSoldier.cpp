#include "j1App.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1GameLayer.h"
#include "GreenSoldier.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"



bool GreenSoldier::Spawn(std::string file, iPoint pos)
{
	bool ret = true;

	//set position
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
		pugi::xml_node attributes = attributesFile.child("attributes").child("green_soldier");

		LoadAttributes(attributes);
		
	}
	return ret;
}



/*bool GreenSoldier::Draw()
{
	bool ret = true;

	currentAnim = &anim.find({ actionState, currentDir })->second;
	sprite->updateSprite(currentPos, currentAnim->pivot, currentAnim->getCurrentFrame(), currentAnim->flip);
	App->render->Draw(sprite);

	//for(int i=0, j=path.size();i<j;i++)
	iPoint k= App->map->MapToWorld(path[0].x, path[0].y);
	SDL_RenderDrawLine(App->render->renderer, currentPos.x, currentPos.y,k.x, k.y );

	return ret;
}*/