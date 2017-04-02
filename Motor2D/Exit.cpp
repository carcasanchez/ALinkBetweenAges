#include "Exit.h"
#include "j1App.h"
#include "j1GameLayer.h"
#include "j1EntityManager.h"
#include "Player.h"
#include "j1SceneManager.h"
#include "j1CollisionManager.h"
#include "j1Render.h"


Exit::Exit() :
	currentPos({ 0,0 }),
	locked(false),
	destPos({ 0,0 }),
	sprite(NULL),
	col(NULL),
	colPivot({ 0,0 })
{
	destiny.assign("");
}

Exit::~Exit() {}

bool Exit::Spawn(DIRECTION direction, iPoint pos, std::string dest, iPoint dPos)
{
	bool ret = true;

	currentPos = pos;
	dir = direction;
	destiny = dest;
	destPos = dPos;

	// TODO: load sprite

	colPivot = { 16, 16 };
	col = App->collisions->AddCollider({ currentPos.x, currentPos.y, 16, 16 }, COLLIDER_TYPE(5), ((j1Module*)App->game));

	return ret;
}

void Exit::Update()
{
	if (col->CheckCollision(App->game->em->player->col->rect) && !locked)
	{
		App->sceneM->RequestSceneChange(destiny, destPos);
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