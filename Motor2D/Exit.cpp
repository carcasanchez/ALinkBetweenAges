#include "Exit.h"
#include "j1App.h"
#include "j1GameLayer.h"
#include "j1EntityManager.h"
#include "Player.h"
#include "j1SceneManager.h"
#include "j1CollisionManager.h"
#include "j1Render.h"


Exit::Exit() :
	exitDest(0),
	dir(DIRECTION(0)),
	locked(false),
	sprite(NULL),
	col(NULL)
{
	destiny.assign("");
}

Exit::~Exit() 
{
	col->to_delete = true;
}

bool Exit::Spawn(std::string dest, int destExit, SDL_Rect pos, DIRECTION d)
{
	bool ret = true;

	destiny = dest;
	exitDest = destExit;
	rect = pos;
	dir = d;

	col = App->collisions->AddCollider(rect, COLLIDER_TYPE(5), ((j1Module*)App->game));

	return ret;
}

void Exit::Update()
{
	if (col->CheckCollision(App->game->em->player->col->rect) && !locked)
	{
		App->sceneM->RequestSceneChange(this);
	}
}

void Exit::Draw()
{

}

bool Exit::CleanUp()
{
	bool ret = true;

	return ret; 
}